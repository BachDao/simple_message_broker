//
// Created by Bach Dao.
//

#include "msg_broker/block.h"

namespace msg_broker::impl {
block::block(size_t size)
    : size_(size), storage_(std::make_unique<std::byte[]>(size)) {}

size_t block::size() const { return size_; }

std::byte *block::data() { return storage_.get(); }
} // namespace msg_broker::impl