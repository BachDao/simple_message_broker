//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_FORWARD_DECLARATIONS_H
#define SIMPLE_BROKER_FORWARD_DECLARATIONS_H

namespace msg_broker {
class conn_old;

namespace client {
class client;
} // namespace client

namespace impl {
class topic_manager;
class topic;
class connection;
class message_handler;
class server_message_handler;
class block;
} // namespace impl
} // namespace msg_broker
#endif // SIMPLE_BROKER_FORWARD_DECLARATIONS_H
