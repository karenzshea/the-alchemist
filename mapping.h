#pragma once

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace mapping {

// Returns a read only mmaped file region on which you can call .get_address() and .get_size()
inline auto fromReadOnlyFile(const std::string& file_path) {
  using namespace boost::interprocess;

  const file_mapping mapping{file_path.c_str(), read_only};
  mapped_region region{mapping, read_only};
  region.advise(mapped_region::advice_willneed);

  return region;
}
}
