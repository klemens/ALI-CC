#include "ValueParsers.h"

#include <algorithm>
#include "WARCException.h"

std::string Value::parseId(std::string uri) {
    if(uri.size() != 47) {
        throw WARC::Exception(std::string("Invalid uuid uri: ").append(uri));
    }

    // 0--------9        18   23   28   33           46
    // <urn:uuid:54231d72-1d15-411b-b282-3f21b8d0ce2f>
    uri.erase(46, 1);
    uri.erase(33, 1);
    uri.erase(28, 1);
    uri.erase(23, 1);
    uri.erase(18, 1);
    uri.erase(0, 10);

    auto nonHexChar = std::find_if_not(uri.begin(), uri.end(), [](char c) {
        return ('0' <= c && c <= '9') ||
               ('A' <= c && c <= 'F') ||
               ('a' <= c && c <= 'f');
    });
    if(nonHexChar != uri.end()) {
        throw WARC::Exception(std::string("Invalid char in id: ")
                              .append(std::to_string(*nonHexChar)));
    }

    return uri;
}
