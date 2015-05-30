#include "WARCRecord.h"
#include "WARCReader.h"

#include <sstream>

void readHeaders(std::istream& input, WARC::Record<void>& record) {
    WARC::Reader::parseHeaders(input, record.headers);

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

void readContent(std::istream& input, WARC::Record<void>& record) {
    input.ignore(record.length);
}
