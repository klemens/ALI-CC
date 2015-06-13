#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"
#include "WARCException.h"
#include "CSVWriter.h"
#include "ValueParsers.h"
#include "tclap/CmdLine.h"
#include "rapidjson/Pointer.h"
#include "rapidjson/StringBuffer.h"

void writeCSVHeader(CSV::Writer&);
void processWARC(std::istream&, CSV::Writer&, int);
const rapidjson::Value& extract(const rapidjson::Document&, const rapidjson::Pointer&);

constexpr int noVerbosity = 0;
constexpr int lowVerbosity = 1;
constexpr int medVerbosity = 2;
constexpr int highVerbosity = 3;

int main(int argc, char** argv) {
    std::istream* input;
    std::ostream* output;

    TCLAP::CmdLine cmd("meta-extractor", ' ', "0.1");
    TCLAP::ValueArg<std::string> inFile("i", "input", "Input file", false, "", "file", cmd);
    TCLAP::ValueArg<std::string> outFile("o", "output", "Output file", false, "", "file", cmd);
    TCLAP::MultiSwitchArg verbosity("v", "verbose", "Verbose Output", cmd, noVerbosity);
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
        processWARC(*input, csv, verbosity.getValue());
    } catch(const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 10;
    }

    return 0;
}

void writeCSVHeader(CSV::Writer& csv) {
    csv
        << "id"                 // string : 128 bit hex string
        << "timestamp"          // string : iso 8601 date
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

void processWARC(std::istream& input, CSV::Writer& writer, int verbosity) {
    using Pointer = rapidjson::Pointer;

    WARC::Reader reader(input);
    WARC::Record<rapidjson::Document> record;
    uint32_t countProcessed {0}, countIgnored {0};

    const Pointer pType("/Envelope/WARC-Header-Metadata/WARC-Type");
    const Pointer pRecordId("/Envelope/WARC-Header-Metadata/WARC-Record-ID");

    while(reader.read(record)) {
        std::string contentType = extract(record.content, pType).GetString();

        if (contentType == "response") {
            ++countProcessed;

            writer << Value::parseId(extract(record.content, pRecordId).GetString());
            writer << record.date;

            writer.next();
        } else {
            ++countIgnored;
        }

        if (verbosity >= highVerbosity) {
            std::cerr << record.id << ", " << record.date << ", "
                      << record.length << " bytes, "
                      << contentType << std::endl;
        }
    }

    if(verbosity >= lowVerbosity) {
        std::cerr << countProcessed << " records processed, "
                  << countIgnored << " records ignored because of Content-Type"
                  << std::endl;
    }
}

const rapidjson::Value& extract(const rapidjson::Document& doc, const rapidjson::Pointer& pointer) {
    if(auto value = pointer.Get(doc)) {
        // Returning a reference is ok, because the object is stored in the doc
        return *value;
    } else {
        rapidjson::StringBuffer pointerString;
        pointer.Stringify(pointerString);
        throw WARC::Exception(std::string("Invalid WAT: missing entry ")
                              .append(pointerString.GetString()));
    }
}
