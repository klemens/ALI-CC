#include "WARCRecord.h"

#include <istream>
#include "WARCException.h"
#include "rapidjson/error/en.h"

bool WARC::readContent(std::istream& input, WARC::Record<void>& record) {
    input.ignore(record.length);

    return true;
}

bool WARC::readContent(std::istream& input, WARC::Record<rapidjson::Document>& record) {
    // only read and parse actual json
    if(record.headers.find("Content-Type") == record.headers.end() ||
       record.headers["Content-Type"] != "application/json") {
        input.ignore(record.length);
        return false;
    }

    // resize buffer if necessary
    if(record.length >= record.buffer_size) {
        // resize buffer to at least 5 MiB
        size_t new_buffer_size = std::max<size_t>(record.length + 1, (1ull<<20) * 5);
        record.buffer = std::unique_ptr<char[]>(new char[new_buffer_size]);
        record.buffer_size = new_buffer_size;
    }

    // read complete json into char buffer
    if(!input.read(record.buffer.get(), record.length)) {
        throw WARC::Exception(std::string("Invalid WARC: read ended premature, "
                              "probably wrong Content-Size in record at ")
                              .append(std::to_string(input.tellg())));
    }
    record.buffer[record.length] = '\0';

    if(record.content.ParseInsitu(record.buffer.get()).HasParseError()) {
        throw WARC::Exception(std::string("Invalid WARC: malformed json (")
                              .append(rapidjson::GetParseError_En(
                                      record.content.GetParseError()))
                              .append(") at ")
                              .append(std::to_string(input.tellg())));
    }

    return true;
}
