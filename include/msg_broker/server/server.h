//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_SERVER_H
#define SIMPLE_BROKER_SERVER_H
#include "boost/asio.hpp"
#include "msg_broker/forward_declarations.h"
#include "msg_broker/server/server_message_handler.h"
#include "msg_broker/server/server_topic_manager.h"

namespace msg_broker::server {

class server {
  int port_;
  int workersNum_;
  boost::asio::io_context ioCtx_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::vector<std::thread> workers_;
  server_topic_manager tpManager_;
  std::shared_ptr<impl::message_handler> ptrMessageHandler_ =
      std::make_shared<server_message_handler>(tpManager_);

  void accept_connection();
  void handle_new_connection(boost::asio::ip::tcp::socket &&peer);

public:
  server(int port, int workers = std::thread::hardware_concurrency());
  ~server();
};

} // namespace msg_broker::server

#endif // SIMPLE_BROKER_SERVER_H
