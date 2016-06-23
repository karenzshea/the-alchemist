#pragma once

#include <cstdint>

// Integral tags used in Protobuf messages.
// See corresponding *.proto schema definition.

namespace tags {

enum Header : std::uint32_t {
  groupSizeInBytes = 1,
};

enum Group : std::uint32_t {
  fromNodes = 1,
  toNodes = 2,
  speeds = 3,
};

}
