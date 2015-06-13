#ifndef _WARCRECORD_H_
#define _WARCRECORD_H_

#include <string>
#include <unordered_map>
#include <memory>
#include "rapidjson/document.h"

namespace WARC {
    typedef std::unordered_map<std::string, std::string> header_t;

    struct RecordBase {
        std::string id;
        std::string type;
        std::string date;
        header_t headers;
        size_t length {0};
        void clear() {
            id.clear();
            type.clear();
            date.clear();
            headers.clear();
            length = 0;
        }
    };

    template<typename Content>
    struct Record : RecordBase {
        Content content;
    };

    template<>
    struct Record<void> : RecordBase {};
    bool readContent(std::istream& input, Record<void>& record);

    template<>
    struct Record<rapidjson::Document> : RecordBase {
        rapidjson::Document content;
        std::unique_ptr<char[]> buffer {nullptr};
        size_t buffer_size {0};
        void parseJson();
    };
    bool readContent(std::istream& input, Record<rapidjson::Document>& record);
}

#endif
