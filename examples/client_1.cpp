//
// Created by Bach Dao.
//
#include "msg_broker/client/client.h"

#include <iostream>
#include <string>

using namespace msg_broker::client;
using namespace std::chrono;

int main() {
  try {

    auto ptrClient = client::connect_to("127.0.0.1", 3000, "client_1");
    if (!ptrClient) {
      std::cout << "can't connect to server" << std::endl;
      exit(1);
    }

    auto ptrTopic = ptrClient->subscribe("topic_name");

    ptrTopic->on_message(
        [](std::string msg) { std::cout << "receive message" << std::endl; });

    ptrClient->start();

    int counter = 0;
    while (true) {
      std::this_thread::sleep_for(500ms);
      if (!ptrTopic->send_message("counter" + std::to_string(counter++)))
        break;
    }
  } catch (...) {
  }
  return 0;
}