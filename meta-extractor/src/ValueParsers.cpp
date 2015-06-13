#include "ValueParsers.h"

#include <algorithm>

std::string Value::parseId(std::string uri) {
    auto newEnd = std::remove_if(uri.begin(), uri.end(), [](char c) {
        return !(('0' <= c && c <= '9') ||
                 ('A' <= c && c <= 'F') ||
                 ('a' <= c && c <= 'f'));
    });
    // std::remove_if only moves around values and does not change the size
    uri.erase(newEnd, uri.end());
    return uri;
}
