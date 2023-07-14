//
// Created by Bach Dao.
//

#include "msg_broker/server/server_topic.h"

#include "msg_broker/connection.h"

#include <iostream>

namespace msg_broker::server {

server_topic::server_topic(std::string topicName) : topicName_(topicName) {}

void server_topic::subscribe(
    std::shared_ptr<msg_broker::impl::connection> ptr) {
  std::lock_guard lck(mutex_);
  auto connId = ptr->get_id();
  connections_.insert_or_assign(connId, ptr);
}

void server_topic::unsubscribe(
    std::shared_ptr<msg_broker::impl::connection> ptr) {
  std::lock_guard lck(mutex_);
  auto connId = ptr->get_id();
  connections_.erase(connId);
}

size_t server_topic::size() {
  std::lock_guard lck(mutex_);
  return connections_.size();
}

bool server_topic::contains(const std::string &clientId) {
  std::lock_guard lck(mutex_);
  return connections_.contains(clientId);
}

void server_topic::dispatch(
    const std::string &senderId_,
    std::shared_ptr<msg_broker::impl::block> ptrDataFrame) {

  std::lock_guard lck(mutex_);
  std::vector<std::string> closedConnections_;

  for (auto &[k, v] : connections_) {
    auto ptrConn = v.lock();

    // remove later
    if (!ptrConn) {
      closedConnections_.push_back(k);
      continue;
    }

    if (ptrConn->get_id() != senderId_) {
      ptrConn->send(ptrDataFrame);
    }
  }
  // remove terminated connection
  for (auto &c : closedConnections_) {
    connections_.erase(c);
  }
}

} // namespace msg_broker::server