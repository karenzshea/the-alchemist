#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include <protozero/pbf_writer.hpp>
#include <protozero/varint.hpp>

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    using Buffer=std::string;
    Buffer thebytes;
    protozero::pbf_writer bytespbf(thebytes);

    bytespbf.add_uint64(1, 42);
    bytespbf.add_uint64(2, 590);
    bytespbf.add_uint32(3, 60);

    Buffer thesize;
    protozero::write_varint(std::back_inserter(thesize), 3);
    protozero::write_varint(std::back_inserter(thesize), 1);

    const auto count = ::fwrite(thebytes.data(), sizeof(char), thebytes.size(), testOutputFile.get());
    if (count != thebytes.size()) std::quick_exit(EXIT_FAILURE);
    ::puts("tada\n");
}
