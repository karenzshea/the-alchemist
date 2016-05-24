#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include <protozero/pbf_writer.hpp>

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);

    using Buffer=std::string;
    Buffer thebytes;
    protozero::pbf_writer pbf(thebytes);

    pbf.add_uint64(1, 42);
    pbf.add_uint64(2, 590);
    pbf.add_uint32(3, 60);
    const auto count = ::fwrite(thebytes.data(), sizeof(char), thebytes.size(), testOutputFile.get());
    if (count != thebytes.size()) std::quick_exit(EXIT_FAILURE);
    ::puts("tada\n");
}
