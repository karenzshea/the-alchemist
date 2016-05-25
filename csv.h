#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#include <istream>
#include <utility>
#include <string>

#include <boost/fusion/include/adapted.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/spirit/include/qi.hpp>


namespace csv {
struct Line {
  std::uint64_t from;
  std::uint64_t to;
  std::uint32_t speed;
};

using boost::fusion::operator<<;
}

BOOST_FUSION_ADAPT_STRUCT(csv::Line, from, to, speed)


// Takes a stream and a function that expects a Line as argument and runs the function for each csv line
namespace csv {
template <typename Fn>
void forEachLine(std::istream& stream, Fn fn) {
  for (std::string line; std::getline(stream, line);) {
    auto it = begin(line);
    const auto last = end(line);

    csv::Line into;

    using namespace boost::spirit::qi;
    const auto ok = parse(it, last, (ulong_long >> ',' >> ulong_long >> ',' >> ulong_), into);

    if (!ok || it != last) {
      std::fprintf(stderr, "Error: unable to parse file\n");
      std::quick_exit(EXIT_FAILURE);
    }

    (void)fn(std::move(into));
  }
}
}
