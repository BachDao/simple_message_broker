//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_BLOCK_H
#define SIMPLE_BROKER_BLOCK_H
#include <memory>

namespace msg_broker::impl {
class block {
  size_t size_;
  std::unique_ptr<std::byte[]> storage_;

public:
  block(size_t size);
  size_t size() const;
  std::byte *data();
};
} // namespace msg_broker::impl

#endif // SIMPLE_BROKER_BLOCK_H
