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
     * Canonicalize the HTTP Server header to the most used servers and remove
     * version information. Returns "other" for other servers and an empty
     * string if given an empty string
     * eg: Nginx/1.9.2 -> nginx
     * eg: My cool server -> *empty string*
     */
    std::string canonicalizeServer(const std::string& server);
}

#endif
