#include "WARCReader.h"

#include <sstream>
#include "WARCException.h"

WARC::Reader::Reader(std::istream& input) : input(input) {}

bool WARC::Reader::hasNextRecord() {
    return !(input.peek() == EOF && input.eof());
}

void WARC::Reader::checkLine(const std::string& expectedLine) {
    std::string line = readLine(input);
    if(line != expectedLine) {
        throw WARC::Exception(std::string("Invalid WARC: read '")
                              .append(line)
                              .append("', expected '")
                              .append(expectedLine)
                              .append("' at ")
                              .append(std::to_string(input.tellg())));
    }
}

std::string WARC::Reader::readLine(std::istream& input) {
    std::string line;
    std::getline(input, line);

    if(line.back() != '\r') {
        throw WARC::Exception(std::string("Invalid WARC: line '")
                              .append(line)
                              .append("' at ")
                              .append(std::to_string(input.tellg()))
                              .append(" does not end with \\r\\n"));
    }
    // trim trailing \r
    line.pop_back();

    return line;
}

void WARC::Reader::parseHeaders(std::istream& input, WARC::RecordBase& record) {
    std::string lastKey;

    while(true) {
        std::string header = WARC::Reader::readLine(input);
        if(header.empty()) {
            break;
        }

        // headers can span multiple lines when prepended with SP or HT
        size_t beginKey = header.find_first_not_of(" \t");
        if(beginKey == 0) {
            // new header field
            std::string key, value;

            size_t separator = header.find_first_of(':');
            if(separator == std::string::npos || separator == 0) {
                throw WARC::Exception(std::string("Invalid WARC header: '")
                                      .append(header)
                                      .append("' at ")
                                      .append(std::to_string(input.tellg())));
            }
            key = header.substr(0,separator);

            size_t beginValue = header.find_first_not_of(" \t", separator + 1);
            if(beginValue == std::string::npos) {
                // empty value (probably continued on next line)
            } else {
                value = header.substr(beginValue);
            }

            record.headers.insert(std::make_pair(key, value));
            lastKey = key;
        } else if(beginKey == std::string::npos) {
            throw WARC::Exception(std::string("Invalid WARC header: '")
                                  .append(header)
                                  .append("' at ")
                                  .append(std::to_string(input.tellg())));
        } else {
            // continuation needs previously inserted header
            if(record.headers.find(lastKey) == record.headers.end()) {
                throw WARC::Exception(std::string("Invalid WARC header: '")
                                      .append(header)
                                      .append("' at ")
                                      .append(std::to_string(input.tellg())));
            }
            record.headers[lastKey].append(header.substr(beginKey));
        }
    }

    // copy over the default attributes
    auto id = record.headers.find("WARC-Record-ID");
    auto type = record.headers.find("WARC-Type");
    auto date = record.headers.find("WARC-Date");
    auto length = record.headers.find("Content-Length");
    if(id == record.headers.end() || type == record.headers.end() ||
       date == record.headers.end() || length == record.headers.end()) {
        throw WARC::Exception(std::string("Invalid WARC record: missing "
                                          "mandatory headers at ")
                              .append(std::to_string(input.tellg())));
    }

    record.id = id->second;
    record.type = type->second;
    record.date = date->second;

    std::istringstream ss(length->second);
    if (!(ss >> record.length)) {
        throw WARC::Exception(std::string("Invalid WARC Content-Length at ")
                              .append(std::to_string(input.tellg())));
    }
}
