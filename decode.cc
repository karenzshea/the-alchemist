#include <protozero/pbf_reader.hpp>
#include <cstdio>
#include <ios>
#include <iostream>
#include <fstream>

/*
 * Read in pbf, probably copy the csv.h file a bit
 * read header message and set number of bundles to iterate over
 * iterate over array of bundles
 * within a bundle, iterate over lines of pbf messages
 * decoding type based on tag
 * write decoded pbf to csv file
*/

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

}
