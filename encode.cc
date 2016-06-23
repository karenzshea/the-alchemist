#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <protozero/pbf_writer.hpp>
#include <protozero/varint.hpp>

#include "csv.h"
#include "tags.h"

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;
using Buffer = std::string;

const constexpr std::size_t lines_per_bundle = 1000;

void writeBufferToFile(const Buffer &bytes, ::FILE *FileToWrite) {
  const auto count = ::fwrite(bytes.data(), sizeof(char), bytes.size(), FileToWrite);
  if (count != bytes.size()) {
    ::fprintf(stderr, "Error: unable to completely write file\n");
    ::quick_exit(EXIT_FAILURE);
  }
}

void writeHeaderToFile(const std::size_t bundle_count, ::FILE *FileToWrite) {
  auto size = static_cast<std::uint64_t>(bundle_count);
  Buffer buffer;
  auto it = std::back_inserter(buffer);
  protozero::write_varint(it, size);
  writeBufferToFile(buffer, FileToWrite);
}

Buffer encodeGroup(const std::vector<csv::Line> &unencodedLines) {
  Buffer group;
  {
    protozero::pbf_writer groupPbf(group);
    {
      protozero::packed_field_uint64 fromNodes{groupPbf, tags::Group::fromNodes};
      for (const auto &line : unencodedLines)
        fromNodes.add_element(line.from);
    }
    {
      protozero::packed_field_uint64 toNodes{groupPbf, tags::Group::toNodes};
      for (const auto &line : unencodedLines)
        toNodes.add_element(line.to);
    }
    {
      protozero::packed_field_uint32 speeds{groupPbf, tags::Group::speeds};
      for (const auto &line : unencodedLines)
        speeds.add_element(line.speed);
    }
  }
  return group;
}

void writeMessageWithHeaderToFile(const std::vector<csv::Line> &LinesToWrite, ::FILE *FileToWrite) {
  // encode chunked lines into packed fields
  auto bundle = encodeGroup(LinesToWrite);
  // write header file with bundle size
  writeHeaderToFile(bundle.size(), FileToWrite);
  // write bundle to file
  writeBufferToFile(bundle, FileToWrite);
}

int main(int _argc, char **_argv) {
  const std::vector<std::string> args{_argv, _argv + _argc};

  if (args.size() != 2) {
    ::fprintf(stderr, "Usage: %s outfile.pbf\n", args[0].c_str());
    ::quick_exit(EXIT_FAILURE);
  }

  AutoclosingFile testOutputFile(::fopen(args[1].c_str(), "wb"), &::fclose);

  if (!testOutputFile.get()) {
    ::fprintf(stderr, "Error: unable to open file to write\n");
    ::quick_exit(EXIT_FAILURE);
  }

  // write repeated bundles from csv instream
  {
    std::vector<csv::Line> unencodedLines;
    unencodedLines.reserve(lines_per_bundle);

    std::size_t i = 0;

    csv::forEachLine(std::cin, [&](auto &&line) {
      if (i < lines_per_bundle) {
        unencodedLines.push_back(line);
        i++;
      } else {
        writeMessageWithHeaderToFile(unencodedLines, testOutputFile.get());
        // reset
        i = 0;
        unencodedLines.clear();
        unencodedLines.push_back(line);
        i++;
      }
    });

    if (unencodedLines.size() > 0) {
      writeMessageWithHeaderToFile(unencodedLines, testOutputFile.get());
    }
  }
}
