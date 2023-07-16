//
// Created by Bach Dao.
//

#include "msg_broker/server/server_topic_manager.h"

#include "msg_broker/server/server_topic.h"

#include <cassert>
#include <string>

namespace msg_broker::server {

server_topic &server_topic_manager::get_topic(const std::string &tpName) {
  std::lock_guard lck(mutex_);
  auto it = topics_.find(tpName);
  if (it != topics_.end()) {
    return it->second;
  }
  auto [newIt, success] = topics_.emplace(tpName, tpName);
  assert(success);
  return newIt->second;
}

bool server_topic_manager::exist(const std::string &tpName) {
  std::lock_guard lck(mutex_);
  return topics_.contains(tpName);
}
} // namespace msg_broker::server