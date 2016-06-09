#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>
#include <memory>

#include <protozero/pbf_reader.hpp>
#include <protozero/pbf_message.hpp>
#include <protozero/varint.hpp>

#include "mapping.h"

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;
void writetoFile(const std::string& bytes, ::FILE* FileToWrite)
{
    const auto count = ::fwrite(bytes.data(),
            sizeof(char), bytes.size(), FileToWrite);
    if (count != bytes.size()) std::quick_exit(EXIT_FAILURE);
}

int main() {
    AutoclosingFile decodedFile(::fopen("out.csv", "wb"), &::fclose);
    if (!decodedFile) std::quick_exit(EXIT_FAILURE);

    auto infile = mapping::fromReadOnlyFile("out.pbf");

    const char* end = reinterpret_cast<const char*>(infile.get_address())+infile.get_size();
    const char* pos = reinterpret_cast<const char*>(infile.get_address());

    while (pos != end) {
        uint64_t chunksize = protozero::decode_varint(
                &pos, end);

        protozero::pbf_reader bundle(pos, chunksize);
        while (bundle.next(1)) {
            protozero::pbf_reader line = bundle.get_message();
            uint64_t from, to;
            uint32_t speed;
            while (line.next()) {
                switch(line.tag()) {
                    case 1:
                        from = line.get_uint64();
                        writetoFile(std::to_string(from) + ",", decodedFile.get());
                        break;
                    case 2:
                        to = line.get_uint64();
                        writetoFile(std::to_string(to) + ",", decodedFile.get());
                        break;
                    case 3:
                        speed = line.get_uint32();
                        writetoFile(std::to_string(speed) + "\n", decodedFile.get());
                        break;
                    default:
                        line.skip();
                }
            }
        }
        pos += chunksize;
    }
}
