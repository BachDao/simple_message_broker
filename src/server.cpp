//
// Created by Bach Dao.
//

#include "msg_broker/server/server.h"

#include "msg_broker/connection.h"
#include "msg_broker/server/server_message_handler.h"
#include "msg_broker/server/server_topic.h"
using namespace boost::asio::ip;
using namespace boost::system;

namespace msg_broker::server {

void server::accept_connection() {
  acceptor_.async_accept([&](const error_code &erc, tcp::socket sock) {
    if (erc) {
      exit(1);
      return;
    }
    handle_new_connection(std::move(sock));
    accept_connection();
  });
}

server::server(int port, int workers)
    : port_(port), workersNum_(workers), ioCtx_(),
      acceptor_(ioCtx_, tcp::endpoint(tcp::v4(), port_)) {
  accept_connection();
  for (int i = 0; i < workersNum_ - 1; ++i) {
    workers_.emplace_back(std::thread([&] { ioCtx_.run(); }));
  }
  ioCtx_.run();
}

void server::handle_new_connection(boost::asio::ip::tcp::socket &&peer) {
  auto ptrMsgHandler = std::make_shared<server_message_handler>(tpManager_);
  auto ptrConn =
      std::make_shared<impl::connection>(std::move(peer), ptrMsgHandler);
  ptrConn->start();
}

server::~server() {
  for (auto &w : workers_) {
    if (w.joinable())
      w.join();
  }
}

} // namespace msg_broker::server