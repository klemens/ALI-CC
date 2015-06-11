#include <iostream>
#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"
#include "CSVWriter.h"
#include "tclap/CmdLine.h"

void writeCSVHeader(CSV::Writer&);
void processWARC(std::istream&, CSV::Writer&, int);

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

void processWARC(std::istream& input, CSV::Writer&, int verbosity) {
    WARC::Reader reader(input);
    WARC::Record<rapidjson::Document> record;
    uint32_t countProcessed {0}, countIgnored {0};

    while(reader.read(record)) {
        // this is not a json record
        if(!record.valid) {
            record.clear();
            continue;
        }

        std::string content_type = record.content["Envelope"]
                                                 ["WARC-Header-Metadata"]
                                                 ["Content-Type"].GetString();
        // TODO: Field --> "WARC-Type" : "response"

        if (content_type == "application/http; msgtype=response") {
            ++countProcessed;

            // TODO: surround with try-catches for missing fields etc.?

            // TODO: id from "Block-Digest", "WARC-Record-ID"??

            std::string date = record.content["Envelope"]
                                             ["WARC-Header-Metadata"]
                                             ["WARC-Date"].GetString();
            if (verbosity >= highVerbosity) {
                std::cerr << "Date: " << date << std::endl;
            }
            // TODO: parse to unix timestamp

            std::string uri = record.content["Envelope"]
                                            ["WARC-Header-Metadata"]
                                            ["WARC-Target-URI"].GetString();
            if (verbosity >= highVerbosity) {
                std::cerr << "URI: " << uri << std::endl;
            }
            // TODO: parse and split into fragments

            std::string ctype = record.content["Envelope"]
                                              ["Payload-Metadata"]
                                              ["Actual-Content-Type"].GetString();
            ctype = ctype.substr(0, ctype.find(";"));
            if (verbosity >= highVerbosity) {
                std::cerr << "Content-Type: " << ctype << std::endl;
            }
            // TODO: multiple occurances of content type ...

            const rapidjson::Value& headers = record.content["Envelope"]
                                                            ["Payload-Metadata"]
                                                            ["HTTP-Response-Metadata"]
                                                            ["Headers"];
            for (rapidjson::Value::ConstMemberIterator itr = headers.MemberBegin();
                 itr != headers.MemberEnd(); ++itr) {
                std::string header_key = itr->name.GetString();
                if (header_key == "Server") {
                    std::string server = headers[header_key.c_str()].GetString();
                    if (verbosity >= highVerbosity) {
                        std::cerr << "Server: " << server << std::endl;
                    } // if
                    // TODO: parse server / enum list
                } // if
            } // for
            // TODO: other header fields

            const rapidjson::Value& http = record.content["Envelope"]
                                                         ["Payload-Metadata"]
                                                         ["HTTP-Response-Metadata"];
            const rapidjson::Value::ConstMemberIterator& html_iter = http.FindMember("HTML-Metadata");
            if (html_iter != http.MemberEnd()) {
                if (verbosity >= highVerbosity) {
                    std::cerr << "Has >>HTML-Metadata<<" << std::endl;
                } // if
                const rapidjson::Value& html = html_iter->value;
                const rapidjson::Value::ConstMemberIterator& head_iter = html.FindMember("Head");
                if (head_iter != html.MemberEnd()) {
                    if (verbosity >= highVerbosity) {
                        std::cerr << "Has >>Head<<" << std::endl;
                    } // if
                    const rapidjson::Value& head = head_iter->value;
                    const rapidjson::Value::ConstMemberIterator scripts_iter = head.FindMember("Scripts");
                    if (scripts_iter != head.MemberEnd()) {
                        if (verbosity >= highVerbosity) {
                            std::cerr << "Has >>Scripts<<" << std::endl;
                        } // if
                        const rapidjson::Value& scripts = scripts_iter->value;
                        for (rapidjson::SizeType i = 0; i < scripts.Size(); ++i) {
                            std::string script = scripts[i]["url"].GetString();
                            // TODO: compare to list of popular scripts
                        } // for
                        if (verbosity >= highVerbosity) {
                            std::cerr << "#Scripts: " << scripts.Size() << std::endl;
                        } // if
                    } else {
                        if (verbosity >= highVerbosity) {
                            std::cerr << "Has no >>Scripts<<" << std::endl;
                        } // if
                    } // if

                    const rapidjson::Value::ConstMemberIterator metas_iter = head.FindMember("Metas");
                    if (metas_iter != head.MemberEnd()) {
                        if (verbosity >= highVerbosity) {
                            std::cerr << "Has >>Metas<<" << std::endl;
                        } // if
                        const rapidjson::Value& metas = metas_iter->value;
                        for (rapidjson::SizeType i = 0; i < metas.Size(); ++i) {
                            const rapidjson::Value& meta = metas[i];
                            // TODO: check if "HasMember()" is really working!!!
                            if (meta.HasMember("content-type")) {
                                std::string ctype_encoding = meta["content"].GetString();
                                if (verbosity >= highVerbosity) {
                                    std::cerr << "Encoding/CT: " << ctype_encoding << std::endl;
                                } // if
                            } // if
                        } // for
                    } else {
                        if (verbosity >= highVerbosity) {
                            std::cerr << "Has no >>Metas<<" << std::endl;
                        } // if
                    } // if
                } else {
                    if (verbosity >= highVerbosity) {
                        std::cerr << "Has no >>Head<<" << std::endl;
                    } // if
                } // if
            } else {
                if (verbosity >= highVerbosity) {
                    std::cerr << "Has no >>HTML-Metadata<<" << std::endl;
                } // if
            } // if

        } else {
            ++countIgnored;
        }

        if (verbosity >= highVerbosity) {
            std::cerr << record.id << ", " << record.date << ", "
                      << record.length << " bytes, "
                      << content_type << std::endl;
        }

        // clear record because it is reused
        record.clear();
    }

    if(verbosity >= lowVerbosity) {
        std::cerr << countProcessed << " records processed, "
                  << countIgnored << " records ignored because of Content-Type"
                  << std::endl;
    }
}
