#include "CSVWriter.h"

CSV::Writer::Writer(std::ostream& output) : output(output), separator(',') {}

CSV::Writer::Writer(std::ostream& output, const char separator) : output(output), separator(separator) {}

CSV::Writer CSV::Writer::operator<< (const std::string& data) {
    if (!isFirstValue) {
        output << separator;
    } else {
        isFirstValue = false;
    }

    output << clean(data);

    return *this;
}

CSV::Writer CSV::Writer::next() {
    output << std::endl;

    isFirstValue = true;

    return *this;
}

std::string CSV::Writer::clean(const std::string& str) {
    // TODO
    return str;
}
