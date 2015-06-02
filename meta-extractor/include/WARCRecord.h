#ifndef _WARCRECORD_H_
#define _WARCRECORD_H_

#include <string>
#include <unordered_map>

namespace WARC {
    typedef std::unordered_map<std::string, std::string> header_t;

    struct RecordBase {
        std::string id;
        std::string type;
        std::string date;
        header_t headers;
        size_t length {0};
        bool valid {true};
        void clear() {
            id.clear();
            type.clear();
            date.clear();
            headers.clear();
            length = 0;
            valid = true;
        }
    };

    template<typename Content>
    struct Record : RecordBase {
        Content content;
    };

    template<>
    struct Record<void> : RecordBase {};
    void readContent(std::istream& input, Record<void>& record);
}

#endif
