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

    // write repeated bundles from csv instream
    std::size_t bundle_i = 0;
    {
        std::vector<csv::Line> unencodedLines;

        std::size_t i = 0;
        csv::forEachLine(std::cin, [&](auto&& line) {
            if (i < 1000)
            {
                unencodedLines.push_back(line);
                i++;
            } else {
                // encode chunked lines into messages, and write bundle to file
                Buffer bundle;
                {
                    protozero::pbf_writer bundlepbf(bundle);
                    for (int j = 0; j < unencodedLines.size(); ++j) {
                        encodeLine(bundlepbf, unencodedLines[j]);
                    }
                }
                writetoFile(bundle, testOutputFile.get());
                bundle_i++;
                // reset
                i = 0;
                unencodedLines = {};
                unencodedLines.push_back(line);
            }
        });
        if (unencodedLines.size() > 0)
        {
            // encode chunked lines into messages, and write bundle to file
            Buffer bundle;
            {
                protozero::pbf_writer bundlepbf(bundle);
                for (int j = 0; j < unencodedLines.size(); ++j) {
                    encodeLine(bundlepbf, unencodedLines[j]);
                }
            }
            writetoFile(bundle, testOutputFile.get());
            bundle_i++;
        }
    }
    // overwrite initial bundle_count placeholder

    bundle_count.clear();
    {
        protozero::pbf_writer bundleCountpbf(bundle_count);
        bundleCountpbf.add_fixed64(1, bundle_i);
    }
    fseek(testOutputFile.get(), 0, SEEK_SET);
    writetoFile(bundle_count, testOutputFile.get());
}
