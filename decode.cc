#include <cassert>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>

#include <protozero/pbf_message.hpp>
#include <protozero/pbf_reader.hpp>
#include <protozero/varint.hpp>

#include "bundle_limit.h"
#include "csv.h"
#include "mapping.h"
#include "tags.h"

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;
void writetoFile(const std::string &bytes, ::FILE *FileToWrite) {
  const auto count = ::fwrite(bytes.data(), sizeof(char), bytes.size(), FileToWrite);
  if (count != bytes.size())
    std::quick_exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: provide a pbf file to decode\n e.g.: " << argv[0] << " ./file.pbf" << std::endl;
    std::exit(0);
  }

  // TODO parameterize out file as argv
  AutoclosingFile decodedFile(::fopen("out.csv", "wb"), &::fclose);
  if (!decodedFile)
    std::quick_exit(EXIT_FAILURE);

  auto infile = mapping::fromReadOnlyFile(argv[1]);

  const auto *const end = reinterpret_cast<const char *>(infile.get_address()) + infile.get_size();
  const auto *pos = reinterpret_cast<const char *>(infile.get_address());

  // create vector and allocate the bundle limit size
  std::vector<csv::Line> decodedLines(bundle_limit::limit);
  while (pos != end) {
    uint64_t chunksize = protozero::decode_varint(&pos, end);

    protozero::pbf_reader bundle(pos, chunksize);

    // iterate over message, reading out packed values in the order they were
    // written: from nodes, to nodes and speeds
    std::size_t fromNodesLength = 0;
    bundle.next(tags::Group::fromNodes);
    auto pi = bundle.get_packed_uint64();
    for (const auto &fromVal : pi) {
        decodedLines[fromNodesLength].from = fromVal;
        fromNodesLength++;
    }

    std::size_t toNodesLength = 0;
    bundle.next(tags::Group::toNodes);
    pi = bundle.get_packed_uint64();
    for (const auto &toVal : pi) {
        decodedLines[toNodesLength].to = toVal;
        toNodesLength++;
    }

    std::size_t speedsLength = 0;
    bundle.next(tags::Group::speeds);
    auto speedsField = bundle.get_packed_uint32();
    for (const auto &speedVal : speedsField) {
        decodedLines[speedsLength].speed = speedVal;
        speedsLength++;
    }
    assert(fromNodesLength == toNodesLength);
    assert(toNodesLength == speedsLength);

    // in case this is the flushed message at the end of a file, shrink the vector
    // to num of values decoded to get rid of null preallocated values in the vector
    decodedLines.resize(speedsLength);
    for (const auto &line : decodedLines) {
        writetoFile(std::to_string(line.from) + ",", decodedFile.get());
        writetoFile(std::to_string(line.to) + ",", decodedFile.get());
        writetoFile(std::to_string(line.speed) + "\n", decodedFile.get());
    }

    pos += chunksize;
  }
}
