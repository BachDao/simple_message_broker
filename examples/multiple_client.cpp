//
// Created by Bach Dao.
//
#include "msg_broker/client/client.h"
#include "msg_broker/server/server.h"

using namespace msg_broker;
using namespace std::chrono;

int main() {
  std::thread t([] { server::server s(3000); });

  auto consumer_1 = client::client::connect_to("127.0.0.1", 3000, "client_1");
  auto tp_1 = consumer_1->subscribe("topic_name");
  tp_1->on_message([](const std::string &s) {
    std::cout << "callback fired" << std::endl;
    assert(s == "hello_world");
  });
  consumer_1->start();

  auto producer = client::client::connect_to("127.0.0.1", 3000, "producer");
  auto tp = producer->subscribe("topic_name");
  producer->start();

  for (int i = 0; i < 10; ++i) {
    tp->send_message("hello_world");
  }

  t.join();
}
