#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <iostream>

#include <protozero/pbf_writer.hpp>

#include "csv.h"

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;
using Buffer=std::string;
const constexpr std::size_t lines_per_bundle = 1000;

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

    // write a bundle count with a placeholder 0 to outfile
    Buffer bundle_count;
    {
        protozero::pbf_writer bundleCountpbf(bundle_count);
        bundleCountpbf.add_fixed64(1, 0);
    }
    writetoFile(bundle_count, testOutputFile.get());

    {
        std::array<csv::Line, lines_per_bundle> unencodedLines;
        bool end = false;

        std::size_t i = 1;
        csv::forEachLine(std::cin, [&](auto&& line) {
            end = std::cin.eof();
            std::cout << int(end);
            if ((lines_per_bundle % i == 0) || (end))
            {
                // encode chunked lines into messages, and write bundle to file
                Buffer bundle;
                {
                    protozero::pbf_writer bundlepbf(bundle);
                    for (int j = 0; unencodedLines.size(); ++j) {
                        encodeLine(bundlepbf, unencodedLines[j]);
                    }
                }
                writetoFile(bundle, testOutputFile.get());
                // reset
                i = 0;
                unencodedLines = {};
                unencodedLines[i] = line;
            } else {
                unencodedLines[i] = line;
                i++;
            }
        });
    }

}
