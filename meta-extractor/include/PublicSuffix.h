#ifndef _PUBLICSUFFIX_H_
#define _PUBLICSUFFIX_H_

#include <string>
#include <istream>
#include <unordered_map>
#include <memory>

/**
 * Lookup table for public domain suffixes
 *
 * Implements a trie that uses a unordered_map (hash based) to store complete
 * domain parts at each level.
 * Note that this is not highly optimized for lookup speed, however it is
 * fast enough for many use cases. (500,000 to 1,500,000 lookups per second)
 */
class PublicSuffix {
    public:
        /**
         * Constructs the public suffix lookup table using the given
         * public suffix list in standard format.
         * @see: https://publicsuffix.org/list/
         */
        PublicSuffix(std::istream& publicSuffixList);

        /**
         * Get the public suffix for the given domain or an empty string if the
         * domain does not have a valid public suffix / tld.
         * eg: amazon.com -> com
         * eg: amazon.co.uk -> co.uk
         * eg: i.love.you.com -> com
         * eg: klemens.github.io -> github.io
         * eg: example.test -> *empty string*
         */
        std::string getSuffix(const std::string& url) const;

    protected:
        struct Node {
            // exceptions from wildcard (*) rules
            bool exception{false};
            // set to false for intermediate suffix parts
            // eg: amazonaws in s3.amazonaws.com -> amazonaws.com is no suffix
            bool suffix{false};
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        } list;

    private:
        void parseList(std::istream& publicSuffixList);
};

#endif
