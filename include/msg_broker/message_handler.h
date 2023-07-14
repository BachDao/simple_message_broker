//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_MESSAGE_HANDLER_H
#define SIMPLE_BROKER_MESSAGE_HANDLER_H
#include "message.pb.h"
#include "msg_broker/forward_declarations.h"
#include "msg_broker/server/server_topic_manager.h"

#include <memory>
#include <span>

namespace msg_broker {
namespace impl {

class message_handler {
protected:
  connection *ptrConn_;

public:
  // called by connection object, dataFrame contain Message plus Length header
  virtual bool handle_message(std::shared_ptr<block> dataFrame) = 0;
  void set_connection(connection *ptrConn);
  virtual ~message_handler() = default;
};


} // namespace impl
} // namespace msg_broker

#endif // SIMPLE_BROKER_MESSAGE_HANDLER_H
