#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"
#include "tclap/CmdLine.h"

void processWARC(std::istream&, std::ostream&);

int main(int argc, char** argv) {
    std::istream* input;
    std::ostream* output;

    TCLAP::CmdLine cmd("meta-extractor", ' ', "0.1");
    TCLAP::ValueArg<std::string> inFile("i", "input", "Input file", false, "", "file", cmd);
    TCLAP::ValueArg<std::string> outFile("o", "output", "Output file", false, "", "file", cmd);
    try {
        cmd.parse(argc, argv);
    } catch(TCLAP::ArgException &e) {
        std::cerr << "command line parsing error: " << e.what() << std::endl;
        return 1;
    }

    std::ifstream inFileStream;
    if(inFile.isSet()) {
        inFileStream.open(inFile.getValue(), std::ifstream::binary);
        if(!inFileStream.good()) {
            std::cerr << "input file not found" << std::endl;
            return 2;
        }
        input = &inFileStream;
    } else {
        input = &std::cin;
    }

    std::ofstream outFileStream;
    if(outFile.isSet()) {
        outFileStream.open(outFile.getValue(), std::ifstream::binary);
        if(!outFileStream.good()) {
            std::cerr << "output file not found" << std::endl;
            return 3;
        }
        output = &outFileStream;
    } else {
        output = &std::cout;
    }

    try {
        processWARC(*input, *output);
    } catch(const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 10;
    }

    return 0;
}

void processWARC(std::istream& input, std::ostream& output) {
    WARC::Reader reader(input);
    WARC::Record<rapidjson::Document> record;
    size_t count = 0;
    uint64_t maxLength = 0;

    while(reader.read(record)) {
        // this is not a json record
        if(!record.valid) {
            record.clear();
            continue;
        }

        ++count;
        maxLength = std::max(maxLength, record.length);
        output << record.id << ", " << record.date << ", "
               << record.length << " bytes, ";

        output << record.content["Envelope"]
                                ["WARC-Header-Metadata"]
                                ["Content-Type"].GetString()
               << std::endl;

        // clear record because it is reused
        record.clear();
    }

    std::cerr << count << " records, max " << maxLength << " bytes" << std::endl;
}
