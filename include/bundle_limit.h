#include <stdint.h>

// Unscientific tests have found that ~6.5mil lines of input csv that follow schema
// outlined in the .proto file comes in around 57MB. This is an OK limit to
// start, if we want to be under the 64MB protobuf message limit with some room
// TODO: establish a reliable, non-heuristic limit?

namespace bundle_limit {
  const constexpr std::size_t limit = 6500000;
}
