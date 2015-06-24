#ifndef _VALUEPARSERS_H_
#define _VALUEPARSERS_H_

#include <string>

namespace Value {
    /**
     * Parse UUID uri to 128 bit hex number
     * eg: <urn:uuid:ffbfb0c0-6456-42b0-af03-3867be6fc09f>
     *     -> ffbfb0c0645642b0af033867be6fc09f
     */
    std::string parseId(std::string uri);

    /**
     * Extract tld from the given hostname
     * eg: amazon.co.uk
     *     -> uk
     */
    std::string extractTld(const std::string& hostname);

    /**
     * Extract path depth from givon path
     * eg: amazon.co.uk
     *     -> uk
     */
    uint8_t extractPathDepth(const std::string& path);

    /**
     * Parses the MIME type from the "Content-Encoding" property of the HTML header.
     * eg: "text/html; charset=UTF-8" -> "text/html"
     * eg: "text/html" -> "text/html"
     */
    std::string extractMIME(const std::string& contentType);

    /**
     * Extracts the charset (encoding) from the given HTTP Content-Type
     * eg: "text/html; charset=UTF-8" -> "utf-8"
     * eg: "text/html" -> ""
     */
    std::string extractCharset(const std::string& contentType);

    /**
     * Canonicalize the charset to the most used ones. Returns "other" for
     * other charset and an empty string if given one.
     * eg: utf-8 -> utf
     * eg: utf-16 -> utf
     * eg: windows-1256 -> windows
     * eg: iso-8859-1 -> iso
     * eg: tis-620 -> other
     */
    std::string canonicalizeCharset(const std::string& charset);

    /**
     * Canonicalize the HTTP Server header to the most used servers and remove
     * version information. Returns "other" for other servers and an empty
     * string if given an empty string
     * eg: Nginx/1.9.2 -> nginx
     * eg: My cool server -> *empty string*
     */
    std::string canonicalizeServer(const std::string& server);

    /**
     * Check if the given ip looks like ipv6
     * Note: does not perform a syntax check!
     * eg: 10.0.23.19 -> false
     * eg: 82.44.120.203:8080 -> false
     * eg: 2001:a30::11 -> true
     */
    bool isIPv6(const std::string& ip);

    /**
     * Determine if the request uses any kind of compression
     * Content-Encoding: deflate -> true
     */
    bool usesCompression(const std::string& contentEncoding);

    /**
     * Checks whether the URL contains a known CDN.
     */
    bool checkCDN(const char* url);

    /**
     * Check if string starts with the given prefix
     */
    bool prefix(const char* string, const char* prefix);

    /**
     * Check if the given strings are equal ignoring case
     */
    bool equalsI(const char* a, const char* b);
}

#endif
