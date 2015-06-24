#include "ValueParsers.h"

#include <algorithm>
#include <vector>
#include <map>
#include <locale>
#include <cstring>
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

std::string Value::extractMIME(const std::string& contentType) {
    size_t semPos = contentType.find(';');
    if (semPos != std::string::npos) {
        return contentType.substr(0, semPos);
    } else {
        return contentType;
    }
}

std::string Value::extractCharset(const std::string& contentType) {
    size_t charsetBegin = contentType.find("charset=");
    if(charsetBegin != std::string::npos) {
        charsetBegin += 8; // sizeof("charset=") - 1
        if(charsetBegin < contentType.size()) {
            size_t charsetEnd = contentType.find_first_of(" \t,;", charsetBegin);
            auto charset = contentType.substr(charsetBegin, charsetEnd - charsetBegin);
            std::transform(charset.begin(), charset.end(), charset.begin(), ::tolower);
            return charset;
        }
    }
    return "";
}

std::string Value::canonicalizeCharset(const std::string& charset) {
    static const std::map<std::string, std::string> canonCharsets = {
        { "utf", "utf" },
        { "iso-8859-", "iso-8859" },
        { "windows-", "windows" },
        { "ascii", "ascii" },
        { "jis", "jp" },
        { "big5", "cn" },
        { "gbk", "cn" },
        { "jp", "jp" },
        { "kr", "kr" }
    };

    if(charset.empty()) {
        return charset;
    }

    for(const auto& canonCharset : canonCharsets) {
        if(charset.find(canonCharset.first) != std::string::npos) {
            return canonCharset.second;
        }
    }

    return "other";
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

bool Value::usesCompression(const std::string& contentEncoding) {
    if(contentEncoding.empty()) {
        return false;
    }
    return contentEncoding.find("gzip") != std::string::npos ||
           contentEncoding.find("deflate") != std::string::npos;
}

bool Value::prefix(const char* string, const char* prefix) {
    while(*prefix) {
        if(*prefix++ != *string++) {
            return false;
        }
    }
    return true;
}

bool Value::equalsI(const char* a, const char* b) {
    static const auto locale = std::locale::classic();
    while(*a && *b) {
        if(std::tolower(*a++) != std::tolower(*b++)) {
            return false;
        }
    }
    // should both be '\0':
    return *a == *b;
}

bool Value::checkCDN(const char* url) {
    static const char* cdns[] = {
        "ajax.googleapis.com",
        "cloudfront.net",
        "code.jquery.com",
        "typekit.net",          // font hosting by Adobe
        "amazonaws.com",
        "cloudflare.com",
        "maxcdn",               // eg maxcdn.bootstrapcdn.com
        "ajax.aspnetcdn.com",
        "edgecastcdn.net",
        "akamaihd.net",
        "cdn.jsdelivr.net",
        "fastly.net",
    };

    if(url == nullptr) {
        return false;
    }

    if(!Value::prefix(url, "http") && !Value::prefix(url, "//")) {
        return false;
    }

    for(const auto& cdn : cdns) {
        if(std::strstr(url, cdn) != nullptr) {
            return true;
        }
    }

    return false;
}
