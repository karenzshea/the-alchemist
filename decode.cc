#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>
#include <memory>

#include <protozero/pbf_reader.hpp>
#include <protozero/pbf_message.hpp>
#include <protozero/varint.hpp>

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

    std::ifstream infile("out.pbf", std::ios::binary | std::ios::ate);
    int infilesize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    if (!infile.good()) {
        std::perror("Reading file failed");
        return EXIT_FAILURE;
    }
    std::string buffer;
    buffer.resize(infilesize, ' ');
    infile.read(const_cast<char*>(buffer.data()), infilesize);

    const char* end = buffer.c_str()+buffer.size();
    const char* pos = buffer.c_str();

    while (pos != end) {
        uint64_t chunksize = protozero::decode_varint(
                &pos, buffer.data()+buffer.size());

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
