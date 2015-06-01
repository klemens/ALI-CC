#include "WARCRecord.h"
#include "WARCReader.h"

void readContent(std::istream& input, WARC::Record<void>& record) {
    input.ignore(record.length);
}
