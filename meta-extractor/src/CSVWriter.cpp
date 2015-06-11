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
    if (str.find(separator) == std::string::npos) {
        return str;
    } else {
        std::string ret(str);

        // Escape possible quotes
        size_t spos = ret.rfind('"');
        while (spos != std::string::npos) {
            ret.insert(spos, 1, '"');
            spos = ret.rfind('"', std::max<size_t>(0, spos - 1));
        }

        ret.push_back('"');
        ret.insert(0, 1, '"');

        return ret;
    }
}
