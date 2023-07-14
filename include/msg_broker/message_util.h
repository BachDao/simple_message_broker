//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_MESSAGE_UTIL_H
#define SIMPLE_BROKER_MESSAGE_UTIL_H
#include "message.pb.h"
#include "msg_broker/block.h"

#include <mutex>
#include <optional>
#include <span>

namespace msg_broker::impl {

class message_util {
public:
  static std::shared_ptr<impl::block> get_connect_message(std::string connId) {
    return serialize(MessageType::CONNECT, connId, "", "");
  }

  static std::shared_ptr<impl::block>
  get_connect_ack_message(std::string connId) {
    return serialize(MessageType::CONNECT_ACK, connId, "", "");
  }

  static std::shared_ptr<impl::block>
  get_disconnect_message(std::string connId) {
    return serialize(MessageType::DISCONNECT, connId, "", "");
  }

  static std::shared_ptr<impl::block> serialize(MessageType type,
                                                std::string connId,
                                                std::string tpName,
                                                const std::string &msg) {
    ProtoMessage protoMessage;
    protoMessage.set_type(type);
    protoMessage.set_client_id(connId);

    if (tpName.size() > 0) {
      protoMessage.set_topic(tpName);
    }

    if (type == MessageType::NORMAL) {
      protoMessage.set_payload(msg);
    }
    auto msgSize = protoMessage.ByteSizeLong() + sizeof(uint32_t);
    auto ptr = std::make_shared<impl::block>(msgSize);
    *reinterpret_cast<uint32_t *>(ptr->data()) = msgSize;
    protoMessage.SerializeToArray(ptr->data() + sizeof(uint32_t), msgSize);
    return ptr;
  }
};

} // namespace msg_broker::impl
#endif // SIMPLE_BROKER_MESSAGE_UTIL_H
