#include "WARCReader.h"

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
