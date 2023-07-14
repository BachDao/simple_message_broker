//
// Created by Bach Dao.
//
//
// Created by Bach Dao.
//
#include "msg_broker/client/client.h"

#include <iostream>
#include <string>

using namespace msg_broker::client;

int main() {
  auto ptrConn = client::connect_to("127.0.0.1", 3000, "client_2");

  if (!ptrConn)
    std::cout << "can't connect to server" << std::endl;
  auto ptrTopic = ptrConn->subscribe("topic_name");

  ptrTopic->on_message([](const std::string &msg) {
    std::cout << "receive message: " << msg << std::endl;
  });
  ptrConn->start();
  return 0;
}