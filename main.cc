#include <memory>
#include <stdio.h>
#include <stdlib.h>

using AutoclosingFile = std::unique_ptr<::FILE, decltype(&::fclose)>;

int main() {
    AutoclosingFile testOutputFile(::fopen("out.pbf", "wb"), &::fclose);
    if (!testOutputFile.get()) std::quick_exit(EXIT_FAILURE);
}
