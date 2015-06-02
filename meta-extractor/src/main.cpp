#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "meta-extractor: Extract meta information from CC WAT files"
            << std::endl << "Usage: " << argv[0] << " *warc-file*" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1], std::ifstream::binary);
    if(!input.good()) {
        std::cerr << "file not found" << std::endl;
        return 2;
    }

    WARC::Reader reader(input);
    WARC::Record<void> record;
    size_t count = 0;
    uint64_t maxLength = 0;
    try {
        while(reader.read(record)) {
            ++count;
            maxLength = std::max(maxLength, record.length);
            std::cout << record.id << ", " << record.date << ", "
                      << record.type << ", " << record.length << " bytes"
                      << std::endl;

            // clear record because it is reused
            record.clear();
        }
        std::cout << std::endl << count << " records, max "
                  << maxLength << " bytes" << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Exception occurred :" << e.what() << std::endl;
        return 10;
    }
}
