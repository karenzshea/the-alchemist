#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>

#include <protozero/pbf_reader.hpp>
#include <protozero/pbf_message.hpp>
#include <protozero/varint.hpp>

int main() {
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

    const char* pos = buffer.c_str();
    uint64_t chunksize = protozero::decode_varint(
            &pos, buffer.data()+buffer.size());
    std::cout << chunksize;
}
