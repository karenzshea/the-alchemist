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
}

void writeHeader(const std::size_t bundle_count, ::FILE* FileToWrite)
{
    auto size = static_cast<std::uint64_t>(bundle_count);
    Buffer buffer;
    auto it = std::back_inserter(buffer);
    protozero::write_varint(it, size);
    writetoFile(buffer, FileToWrite);
}

Buffer encodeGroup(const std::vector<csv::Line>& unencodedLines)
{
    Buffer group;
    {
        protozero::pbf_writer groupPbf(group);
        {
            protozero::packed_field_uint64 fromNodes{groupPbf, 1};
            for (const auto &line : unencodedLines) fromNodes.add_element(line.from);
        }
        {
            protozero::packed_field_uint64 toNodes{groupPbf, 2};
            for (const auto &line : unencodedLines) toNodes.add_element(line.to);
        }
        {
            protozero::packed_field_uint32 speeds{groupPbf, 3};
            for (const auto &line : unencodedLines) speeds.add_element(line.speed);
        }
    }
    return group;
}

void writeMessageWithHeader(const std::vector<csv::Line>& LinesToWrite, ::FILE* FileToWrite)
{
    // encode chunked lines into packed fields
    auto bundle = encodeGroup(LinesToWrite);
    // write header file with bundle size
    writeHeader(bundle.size(), FileToWrite);
    // write bundle to file
    writetoFile(bundle, FileToWrite);
}

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    // write repeated bundles from csv instream
    {
        std::vector<csv::Line> unencodedLines;
        unencodedLines.reserve(lines_per_bundle);

        std::size_t i = 0;
        csv::forEachLine(std::cin, [&](auto&& line) {
            if (i < lines_per_bundle)
            {
                unencodedLines.push_back(line);
                i++;
            } else {
                writeMessageWithHeader(unencodedLines, testOutputFile.get());
                // reset
                i = 0;
                unencodedLines.clear();
                unencodedLines.push_back(line);
                i++;
            }
        });
        if (unencodedLines.size() > 0)
        {
            writeMessageWithHeader(unencodedLines, testOutputFile.get());
        }
    }
}
