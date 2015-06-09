#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"
#include "CSVWriter.h"
#include "tclap/CmdLine.h"

void writeCSVHeader(CSV::Writer&);
void processWARC(std::istream&, CSV::Writer&, bool);

int main(int argc, char** argv) {
    std::istream* input;
    std::ostream* output;

    TCLAP::CmdLine cmd("meta-extractor", ' ', "0.1");
    TCLAP::ValueArg<std::string> inFile("i", "input", "Input file", false, "", "file", cmd);
    TCLAP::ValueArg<std::string> outFile("o", "output", "Output file", false, "", "file", cmd);
    TCLAP::SwitchArg verbose("v", "verbose", "Verbose Output", cmd, false);
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
        CSV::Writer csv(*output);

        writeCSVHeader(csv);
        processWARC(*input, csv, verbose.getValue());
    } catch(const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 10;
    }

    return 0;
}

void writeCSVHeader(CSV::Writer& csv) {
    csv
        << "time"               // uint32 : unix timestamp (of crawl)
        << "protocol"           // bool   : http[s]
        << "sl-domain"          // string : amazon.co.uk
        << "tld"                // string : uk
        << "public suffix"      // string : co.uk
        << "path depth"         // uint8  : (number of slashes)
        << "path length"        // uint16 :
        << "server (all)"       // string : Apache (2.4)
        // TODO: server (name) only as enum value?
        << "server (name)"      // string : Apache
        << "caching"            // ?
        << "cookies"            // uint16 : number of cookies used
        << "mime"               // string : mime-type e. g. text/html
        << "encoding"           // string : e. g. UTF-8
        // TODO: add rest of fields
        << "scripts"            // string : string[] - clustering
        << "styles"             // string : string[] - clustering
        << "cdn"                // bool   : uses CDNs or not
        << "links (intern)"     // uint16
        << "links (extern)";    // uint16

    csv.next();
}

void processWARC(std::istream& input, CSV::Writer&, bool verbose) {
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

        std::string content_type = record.content["Envelope"]
                                                 ["WARC-Header-Metadata"]
                                                 ["Content-Type"].GetString();

        if (content_type == "application/http; msgtype=response") {
            // TODO: Output data
        }

        if (verbose) {
            std::cerr << record.id << ", " << record.date << ", "
                      << record.length << " bytes, "
                      << content_type << std::endl;
        }

        // clear record because it is reused
        record.clear();
    }

    if(verbose) {
        std::cerr << count << " records, max " << maxLength << " bytes" << std::endl;
    }
}
