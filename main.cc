#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <iostream>

#include <protozero/varint.hpp>
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
    const auto count = ::fwrite(bytes.data(),
            sizeof(char), bytes.size(), FileToWrite);
    if (count != bytes.size()) std::quick_exit(EXIT_FAILURE);
    ::puts("tada\n");
}

void writeHeader(const std::size_t bundle_count, ::FILE* FileToWrite)
{
    auto size = static_cast<std::uint64_t>(bundle_count);
    Buffer buffer;
    auto it = std::back_inserter(buffer);
    protozero::write_varint(it, size);
    writetoFile(buffer, FileToWrite);
}

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    // write repeated bundles from csv instream
    {
        std::vector<csv::Line> unencodedLines;

        std::size_t i = 0;
        csv::forEachLine(std::cin, [&](auto&& line) {
            if (i < 1000)
            {
                unencodedLines.push_back(line);
                i++;
            } else {
                // encode chunked lines into messages
                Buffer bundle;
                {
                    protozero::pbf_writer bundlepbf(bundle);
                    for (int j = 0; j < unencodedLines.size(); ++j) {
                        encodeLine(bundlepbf, unencodedLines[j]);
                    }
                }
                // write header file with bundle size
                writeHeader(unencodedLines.size(), testOutputFile.get());
                // write bundle to file
                writetoFile(bundle, testOutputFile.get());
                // reset
                i = 0;
                unencodedLines = {};
                unencodedLines.push_back(line);
            }
        });
        if (unencodedLines.size() > 0)
        {
            // encode chunked lines into messages
            Buffer bundle;
            {
                protozero::pbf_writer bundlepbf(bundle);
                for (int j = 0; j < unencodedLines.size(); ++j) {
                    encodeLine(bundlepbf, unencodedLines[j]);
                }
            }
            // write header file with bundle size
            writeHeader(unencodedLines.size(), testOutputFile.get());
            // write bundle to file
            writetoFile(bundle, testOutputFile.get());
        }
    }
}
