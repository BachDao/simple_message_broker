//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_SERVER_TOPIC_MANAGER_H
#define SIMPLE_BROKER_SERVER_TOPIC_MANAGER_H

#include "msg_broker/forward_declarations.h"
#include "msg_broker/server/server_topic.h"

#include <mutex>
#include <string>

namespace msg_broker::server {

class server_topic_manager {
  std::mutex mutex_;
  std::unordered_map<std::string, server_topic> topics_;

public:
  server_topic &get_topic(const std::string &tpName);
  bool exist(const std::string &tpName);
};

} // namespace msg_broker::server

#endif // SIMPLE_BROKER_SERVER_TOPIC_MANAGER_H
