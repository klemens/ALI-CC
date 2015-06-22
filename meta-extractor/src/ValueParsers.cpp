#include "ValueParsers.h"

#include <algorithm>
#include <vector>
#include <locale>
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

std::string Value::extractTld(const std::string& hostname) {
    auto pos = hostname.find_last_of('.');
    if(pos == std::string::npos) {
        return "";
    } else {
        return hostname.substr(pos + 1);
    }
}

uint8_t Value::extractPathDepth(const std::string& path) {
    return std::count(path.begin(), path.end(), '/');
}

std::string Value::canonicalizeServer(const std::string& server) {
    static const auto locale = std::locale::classic();
    static const std::vector<std::string> canonServers = {
        "apache-coyote",
        "apache",
        "nginx",
        "iis",
        "ats",
        "gws",
        "gse",
        "litespeed",
        "lighttpd",
        "hiawatha",
        "cherokee",
        "tornado",
        "jetty"
    };

    if(server.size() == 0) {
        return "";
    }

    for(const auto& canonServer : canonServers) {
        auto result = std::search(server.begin(), server.end(),
                                  canonServer.begin(), canonServer.end(),
                                  [](const char& a, const char& b) {
                                      return std::tolower(a, locale) == b;
                                  });

        if(result != server.end()) {
            return canonServer;
        }
    }

    return "other";
}

bool Value::isIPv6(const std::string& ip) {
    if(ip.empty()) {
        return false;
    }
    return ip.find('.') == std::string::npos;
}
