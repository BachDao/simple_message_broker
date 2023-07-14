//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_CLIENT_MESSAGE_HANDLER_H
#define SIMPLE_BROKER_CLIENT_MESSAGE_HANDLER_H
#include "msg_broker/client/client_topic.h"
#include "msg_broker/message_handler.h"

namespace msg_broker {
namespace client {

class client_message_handler : public impl::message_handler {
  std::unordered_map<std::string, std::shared_ptr<client_topic>> topics_;

public:
  client_message_handler();

  bool handle_message(std::shared_ptr<impl::block>) override;

  void handle_normal_message(ProtoMessage);

  void update_topic_list(std::string topicName,
                         std::shared_ptr<client_topic> ptrTp);
};

} // namespace client
} // namespace msg_broker

#endif // SIMPLE_BROKER_CLIENT_MESSAGE_HANDLER_H
