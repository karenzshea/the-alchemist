#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <iostream>

#include <protozero/pbf_writer.hpp>

#include "csv.h"

void writetoFile(std::string StrToWrite, ::FILE* FileToWrite);
using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    using Buffer=std::string;
    Buffer thebytes;

    {
        protozero::pbf_writer bytespbf(thebytes);

        bytespbf.add_uint64(1, 42);
        bytespbf.add_uint64(2, 590);
        bytespbf.add_uint32(3, 60);
    }

    Buffer thesize;
    {
        protozero::pbf_writer headerpbf(thesize);
        headerpbf.add_uint32(1, 1); // todo, write bundles

    }

    writetoFile(thesize, testOutputFile.get());
    writetoFile(thebytes, testOutputFile.get());


    csv::forEachLine(std::cin, [](auto&& line) {
      std::cout << "parsed: " << line << std::endl;
    });
}

void writetoFile(std::string StrToWrite, ::FILE* FileToWrite)
{
    const auto count = ::fwrite(StrToWrite.data(), sizeof(char), StrToWrite.size(), FileToWrite);
    if (count != StrToWrite.size()) std::quick_exit(EXIT_FAILURE);
    ::puts("tada\n");
}
