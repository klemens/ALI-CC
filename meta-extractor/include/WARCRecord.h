#ifndef _WARCRECORD_H_
#define _WARCRECORD_H_

#include <string>
#include <unordered_map>

namespace WARC {
    typedef std::unordered_map<std::string, std::string> header_t;

    template<typename Content>
    struct Record {
        std::string id;
        std::string type;
        std::string date;
        header_t headers;
        uint64_t length;
        Content content;
    };
}

#endif
