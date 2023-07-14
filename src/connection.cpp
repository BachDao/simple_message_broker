//
// Created by Bach Dao.
//

#include "msg_broker/connection.h"

#include "msg_broker/message_handler.h"

namespace msg_broker {
namespace impl {

connection::connection(asio::ip::tcp::socket &&sock,
                       std::shared_ptr<message_handler> ptrMsgHandler)
    : sock_((std::move(sock))) {
  ptrMsgHandler_ = std::move(ptrMsgHandler);
  ptrMsgHandler_->set_connection(this);
}

connection::~connection() {}

bool connection::send(std::shared_ptr<block> data) {
  if (stopRequested_) {
    return false;
  }

  {
    std::lock_guard lck(deliveryMutex_);
    sendQueue_.emplace(std::move(data));
    if (sending_)
      return true;
  }

  deliver_data_frame();
  return true;
}

void connection::start() { receive_data(); }

void connection::receive_data() {
  if (stopRequested_)
    return;
  auto buf = get_buffer();
  sock_.async_receive(
      boost::asio::buffer(buf.data(), buf.size()),
      [&, self = shared_from_this()](const boost::system::error_code &erc,
                                     size_t bytesTransferred) {
        if (erc || bytesTransferred == 0) {
          terminate();
          return;
        }
        buff_.write_advance(bytesTransferred);
        handle_incoming_data();
      });
}

// |LENGTH|FRAME_DATA|
void connection::handle_incoming_data() {

  // received LENGTH field
  while (buff_.size() >= SIZE_OF_LEN_HEADER) {
    auto dataReceived = buff_.data();
    auto frameSize = *reinterpret_cast<uint32_t *>(dataReceived.data());

    // received full frame
    if (dataReceived.size() >= frameSize) {

      auto ptrBlk = std::make_shared<block>(frameSize);
      std::copy(dataReceived.data(), dataReceived.data() + frameSize,
                ptrBlk->data());

      // notify buffer
      buff_.read_consume(frameSize);
      auto success = ptrMsgHandler_->handle_message(std::move(ptrBlk));

      // message's format is wrong, or message's type is not expected
      if (!success)
        terminate();
    }
  }
  // continue read data from socket
  receive_data();
}

std::span<std::byte> connection::get_buffer() { return buff_.get_buffer(); }

void connection_buffer::write_advance(size_t bytesWrite) {
  bytesWrite_ += bytesWrite;
}

void connection_buffer::read_consume(size_t bytesRead) {
  bytesRead_ += bytesRead;
  rewind();
}

std::span<std::byte> connection_buffer::get_buffer() {
  return {buffer_ + bytesWrite_, MESSAGE_MAX_SIZE - bytesWrite_};
}

void connection_buffer::rewind() {
  // // only do rewind when we wasted about half of the buffer
  if (bytesRead_ <= MESSAGE_MAX_SIZE / 2)
    return;
  std::copy(buffer_ + bytesRead_, buffer_ + bytesRead_ + bytesWrite_, buffer_);
  bytesWrite_ = bytesWrite_ - bytesRead_;
  bytesRead_ = 0;
}

std::span<std::byte> connection_buffer::data() {
  return {buffer_ + bytesRead_, bytesWrite_ - bytesRead_};
}

size_t connection_buffer::size() const { return bytesWrite_ - bytesRead_; }

void connection::terminate() {
  std::lock_guard lck{mutex_};

  // someone comes before us
  if (stopRequested_)
    return;

  stopRequested_ = true;
  connState_ = connection_state::disconnected;
  sock_.close();
}

void connection::set_state(connection_state state) { connState_ = state; }

const std::string &connection::get_id() { return id_; }

void connection::set_id(const std::string &id) { id_ = id; }

void connection::deliver_data_frame() {
  if (stopRequested_) {
    return;
  }

  {
    std::lock_guard lck{deliveryMutex_};
    if (sendState_.ptrFrame == nullptr) {
      if (sendQueue_.size() == 0) {
        sending_ = false;
        return;
      }
      sending_ = true;
      sendState_.ptrFrame = std::move(sendQueue_.front());
      sendState_.bytesSent_ = 0;
      sendQueue_.pop();
    }
  }

  std::span<std::byte> data{sendState_.ptrFrame->data() + sendState_.bytesSent_,
                            sendState_.ptrFrame->size() -
                                sendState_.bytesSent_};
  sock_.async_send(
      boost::asio::buffer(data.data(), data.size()),
      [&](const boost::system::error_code &erc, size_t bytesTransferred) {
        if (erc) {
          std::cout << "send error" << std::endl;
          terminate();
        }
        sendState_.bytesSent_ += bytesTransferred;
        // finish current frame
        if (sendState_.bytesSent_ == sendState_.ptrFrame->size()) {
          sendState_.ptrFrame = nullptr;
        }
        deliver_data_frame();
      });
}
} // namespace impl
} // namespace msg_broker