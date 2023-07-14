//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_CONNECTION_H
#define SIMPLE_BROKER_CONNECTION_H
#include "msg_broker/block.h"
#include "msg_broker/defs.h"
#include "msg_broker/forward_declarations.h"

#include <boost/asio.hpp>
#include <queue>
#include <span>

namespace msg_broker {
namespace impl {
namespace asio = boost::asio;

struct connection_buffer {
  std::byte buffer_[MESSAGE_MAX_SIZE];
  size_t bytesWrite_ = 0;
  size_t bytesRead_ = 0;

  void write_advance(size_t bytesWrite);
  void read_consume(size_t bytesRead);
  std::span<std::byte> get_buffer();
  void rewind();
  std::span<std::byte> data();
  size_t size() const;
};

enum class connection_state { initial, established, disconnected };

struct send_state {
  std::shared_ptr<block> ptrFrame = nullptr;
  size_t bytesSent_ = 0;
};

class connection
    : public std::enable_shared_from_this<msg_broker::impl::connection> {
  friend class server_message_handler;

  // we use uint32_t as header to denote size of data frame

  std::mutex mutex_;
  std::mutex deliveryMutex_;
  std::string id_;
  bool sending_ = false;
  asio::ip::tcp::socket sock_;
  connection_buffer buff_;
  std::shared_ptr<message_handler> ptrMsgHandler_;
  std::queue<std::shared_ptr<block>> sendQueue_;
  std::atomic<bool> stopRequested_ = false;
  connection_state connState_ = connection_state::initial;
  send_state sendState_;

  void receive_data();
  void handle_incoming_data();
  std::span<std::byte> get_buffer();
  void deliver_data_frame();

public:
  static constexpr int SIZE_OF_LEN_HEADER = sizeof(uint32_t);

  connection(asio::ip::tcp::socket &&sock,
             std::shared_ptr<message_handler> ptrMsgHandler);
  ~connection();
  bool send(std::shared_ptr<block> data);
  void terminate();
  void set_state(connection_state state);
  const std::string &get_id();
  void set_id(const std::string &id);
  void start();
};

} // namespace impl
} // namespace msg_broker

#endif // SIMPLE_BROKER_CONNECTION_H
