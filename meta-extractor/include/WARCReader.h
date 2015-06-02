#ifndef _WARCREADER_H_
#define _WARCREADER_H_

#include <string>
#include <istream>
#include <stdexcept>
#include "WARCRecord.h"

namespace WARC {
    class Reader {
        public:
            Reader(std::istream& input);

            template<typename Content>
            bool read(Record<Content>& record) {
                if(!hasNextRecord()) {
                    return false;
                }

                checkLine("WARC/1.0");
                parseHeaders(input, record);
                readContent(input, record);
                checkLine("");
                checkLine("");

                return true;
            }

            static std::string readLine(std::istream& input);
            static void parseHeaders(std::istream& input, WARC::RecordBase& record);

        private:
            std::istream& input;

            bool hasNextRecord();
            void checkLine(const std::string& line);
    };
}

#endif
