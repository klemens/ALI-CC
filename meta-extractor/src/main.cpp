#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"
#include "WARCException.h"
#include "CSVWriter.h"
#include "ValueParsers.h"
#include "PublicSuffix.h"
#include "PocoUri.h"
#include "tclap/CmdLine.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"

void writeCSVHeader(CSV::Writer&);
void processWARC(std::istream&, CSV::Writer&, const PublicSuffix&, int);
const rapidjson::Value& extract(const rapidjson::Document&, const rapidjson::Pointer&);
const char* extractString(const rapidjson::Document&, const rapidjson::Pointer&);
const char* extractString(const rapidjson::Document&, const rapidjson::Pointer&, const char*);

constexpr int noVerbosity = 0;
constexpr int lowVerbosity = 1;
constexpr int medVerbosity = 2;
constexpr int highVerbosity = 3;

int main(int argc, char** argv) {
    std::istream* input;
    std::ostream* output;

    TCLAP::CmdLine cmd("meta-extractor", ' ', "0.1");
    TCLAP::ValueArg<std::string> inFile("i", "input", "Input file", false, "", "in-file", cmd);
    TCLAP::ValueArg<std::string> outFile("o", "output", "Output file", false, "", "out-file", cmd);
    TCLAP::ValueArg<std::string> suffixFile("s", "public-suffixes", "Public suffix list file: https://publicsuffix.org/list/", true, "", "suffix-file", cmd);
    TCLAP::SwitchArg csvHeader("", "print-header", "Output includes csv header", cmd, false);
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

    std::ifstream suffixFileStream(suffixFile.getValue());
    if(!suffixFileStream.good()) {
        std::cerr << "public suffix list file not found" << std::endl;
        return 4;
    }
    PublicSuffix suffixes(suffixFileStream);

    try {
        CSV::Writer csv(*output);

        if(csvHeader.getValue()) {
            writeCSVHeader(csv);
        }
        processWARC(*input, csv, suffixes, verbosity.getValue());
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
        << "https"              // bool   : http[s]
        << "hostname"           // string : amazon.co.uk
        << "tld"                // string : uk
        << "public suffix"      // string : co.uk
        << "path depth"         // uint8  : number of slashes in path + 1
        << "pathsegment length" // uint16 : length of path + query + fragment
        << "server"             // string : apache
        << "compression"        // bool   : yes/no
        << "cookies"            // bool   : yes/no
        << "mime"               // string : text/html
        << "scripts"            // string : string[] - clustering
        << "styles"             // string : string[] - clustering
        << "cdn"                // bool   : uses CDNs or not
        << "links (intern)"     // uint16
        << "links (extern)";    // uint16

    csv.next();
}

void processWARC(std::istream& input, CSV::Writer& writer, const PublicSuffix& suffixes, int verbosity) {
    using Pointer = rapidjson::Pointer;

    WARC::Reader reader(input);
    WARC::Record<rapidjson::Document> record;
    uint32_t countProcessed {0}, countIgnored {0};

    const Pointer pType("/Envelope/WARC-Header-Metadata/WARC-Type");
    const Pointer pRecordId("/Envelope/WARC-Header-Metadata/WARC-Record-ID");
    const Pointer pServer("/Envelope/Payload-Metadata/HTTP-Response-Metadata/Headers/Server");
    const Pointer pContentEncoding("/Envelope/Payload-Metadata/HTTP-Response-Metadata/Headers/Content-Encoding");
    const Pointer pCookies("/Envelope/Payload-Metadata/HTTP-Response-Metadata/Headers/Set-Cookie");
    const Pointer pContentType("/Envelope/Payload-Metadata/HTTP-Response-Metadata/Headers/Content-Type");

    while(reader.read(record)) {
        std::string contentType = extractString(record.content, pType);

        if (contentType == "response") {
            ++countProcessed;

            writer << Value::parseId(extractString(record.content, pRecordId));
            writer << record.date;

            Poco::URI url(record.headers.at("WARC-Target-URI"));
            writer << std::to_string(url.getScheme() == "https")
                   << url.getHost()
                   << Value::extractTld(url.getHost())
                   << suffixes.getSuffix(url.getHost())
                   << std::to_string(Value::extractPathDepth(url.getPath()))
                   << std::to_string((uint16_t) url.getPathEtc().size());

            writer << Value::canonicalizeServer(extractString(record.content, pServer, ""));

            writer << std::to_string(Value::usesCompression(
                          extractString(record.content, pContentEncoding, "")));

            writer << std::to_string(pCookies.Get(record.content) != nullptr);

            writer << Value::extractMIME(extractString(record.content, pContentType, ""));

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

const char* extractString(const rapidjson::Document& doc, const rapidjson::Pointer& pointer) {
    return extract(doc, pointer).GetString();
}

const char* extractString(const rapidjson::Document& doc, const rapidjson::Pointer& pointer, const char* defaultString) {
    if(auto value = pointer.Get(doc)) {
        return value->GetString();
    } else {
        return defaultString;
    }
}
