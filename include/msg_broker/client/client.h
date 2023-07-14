//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_CLIENT_H
#define SIMPLE_BROKER_CLIENT_H
#include "boost/asio.hpp"
#include "msg_broker/client/client_topic.h"
#include "msg_broker/connection.h"

namespace msg_broker::client {

class client {
  boost::asio::io_context ioCtx_;
  std::shared_ptr<msg_broker::impl::connection> ptrConn_;
  std::shared_ptr<client_message_handler> ptrMsgHandler_;

  client();
  std::thread receiveThread_;
public:
  ~client();
  static std::unique_ptr<client> connect_to(std::string ip, int port,
                                            const std::string &clientId);
  std::shared_ptr<client_topic> subscribe(std::string topicName);
  void start();
};

} // namespace msg_broker::client

#endif // SIMPLE_BROKER_CLIENT_H
