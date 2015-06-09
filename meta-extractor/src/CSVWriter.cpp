#include "CSVWriter.h"

CSV::Writer::Writer(std::ostream& output) : output(output) {}

CSV::Writer CSV::Writer::operator<< (const std::string& data) {
    if (!isFirstValue) {
        output << CSV_SEP;
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
