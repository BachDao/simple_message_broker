//
// Created by Bach Dao.
//

#include "msg_broker/client/client.h"

#include "msg_broker/client/client_message_handler.h"

namespace msg_broker::client {
std::unique_ptr<client>
client::client::connect_to(std::string ip, int port,
                           const std::string &clientId) {
  assert(clientId.size());
  std::unique_ptr<client> ptrClient(new client());

  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip),
                                    port);
  boost::system::error_code erc;
  boost::asio::ip::tcp::socket sock(ptrClient->ioCtx_);
  sock.connect(ep, erc);

  if (erc) {
    return nullptr;
  }
  ptrClient->ptrConn_ = std::make_shared<impl::connection>(
      std::move(sock), ptrClient->ptrMsgHandler_);

  ptrClient->ptrConn_->set_id(clientId);
  ptrClient->ptrConn_->send(
      impl::message_util::get_connect_message(ptrClient->ptrConn_->get_id()));

  return ptrClient;
}

client::client()
    : ioCtx_(), ptrMsgHandler_(std::make_shared<client_message_handler>()) {}

std::shared_ptr<client_topic> client::subscribe(std::string topicName) {
  auto ptrBlk = impl::message_util::serialize(
      MessageType::TOPIC_SUBSCRIBE, ptrConn_->get_id(), topicName, "");
  ptrConn_->send(ptrBlk);

  auto ptrTopic = std::make_shared<client_topic>(ptrConn_, topicName);
  ptrMsgHandler_->update_topic_list(topicName, ptrTopic);
  return ptrTopic;
}

void client::start() {
  receiveThread_ = std::thread([&] {
    ptrConn_->start();
    ioCtx_.run();
  });
}

client::~client() {
  if (receiveThread_.joinable())
    receiveThread_.join();
}
} // namespace msg_broker::client