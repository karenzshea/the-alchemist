#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <iostream>

#include <protozero/pbf_writer.hpp>

#include "csv.h"

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;
using Buffer=std::string;

void encodeLine(protozero::pbf_writer& parentpbf, const csv::Line& lineData)
{
    protozero::pbf_writer linepbf(parentpbf, 1);

    linepbf.add_uint64(1, lineData.from);
    linepbf.add_uint64(2, lineData.to);
    linepbf.add_uint32(3, lineData.speed);
}

void writetoFile(const Buffer& bytes, ::FILE* FileToWrite)
{
    const auto count = ::fwrite(bytes.data(), sizeof(char), bytes.size(), FileToWrite);
    if (count != bytes.size()) std::quick_exit(EXIT_FAILURE);
    ::puts("tada\n");
}

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    Buffer bundle;
    {
        protozero::pbf_writer bundlepbf(bundle);
        csv::forEachLine(std::cin, [&bundlepbf](auto&& line) {
            encodeLine(bundlepbf, line);
        });
    }

    writetoFile(bundle, testOutputFile.get());
}
