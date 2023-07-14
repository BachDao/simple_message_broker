//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_CLIENT_TOPIC_H
#define SIMPLE_BROKER_CLIENT_TOPIC_H
#include "message.pb.h"
#include "msg_broker/connection.h"
#include "msg_broker/message_util.h"

#include <string>

namespace msg_broker::client {
class client_message_handler;

class client_topic {
  friend class client_message_handler;

  std::shared_ptr<msg_broker::impl::connection> ptrConn_;
  std::function<void(const std::string &)> msgHandler_ = nullptr;
  void handle_message(const ProtoMessage &msg);
  std::string topicName_;

public:
  explicit client_topic(std::shared_ptr<msg_broker::impl::connection> ptrConn,
                        const std::string &tpName);
  void on_message(std::function<void(const std::string &)> handler);
  bool send_message(const std::string &msg);
  void unsubscribe();
};
} // namespace msg_broker::client
#endif // SIMPLE_BROKER_CLIENT_TOPIC_H
