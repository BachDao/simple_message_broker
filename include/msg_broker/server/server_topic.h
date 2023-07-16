//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_SERVER_TOPIC_H
#define SIMPLE_BROKER_SERVER_TOPIC_H
#include "msg_broker/forward_declarations.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace msg_broker::server {

class server_topic {
  std::unordered_map<std::string, std::weak_ptr<msg_broker::impl::connection>>
      connections_;

  std::string topicName_;
  std::mutex mutex_;

public:
  explicit server_topic(std::string topicName);
  void subscribe(std::shared_ptr<msg_broker::impl::connection> ptr);
  void unsubscribe(std::shared_ptr<msg_broker::impl::connection> ptr);
  size_t size();
  void dispatch(const std::string &senderId_,
                std::shared_ptr<msg_broker::impl::block> ptrDataFrame);
  bool contains(const std::string &clientId);
};

} // namespace msg_broker::server

#endif // SIMPLE_BROKER_SERVER_TOPIC_H
