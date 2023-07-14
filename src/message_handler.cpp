//
// Created by Bach Dao.
//

#include "msg_broker/message_handler.h"

#include "msg_broker/connection.h"
#include "msg_broker/message_util.h"
#include "msg_broker/server/server_topic.h"

namespace msg_broker::impl {

void message_handler::set_connection(connection *ptrConn) {
  ptrConn_ = ptrConn;
}

} // namespace msg_broker::impl