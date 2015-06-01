#include "WARCRecord.h"

#include <istream>

void WARC::readContent(std::istream& input, WARC::Record<void>& record) {
    input.ignore(record.length);
}
