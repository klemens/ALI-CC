#include "PublicSuffix.h"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> splitString(const std::string&, const char);

PublicSuffix::PublicSuffix(std::istream& publicSuffixList) {
    parseList(publicSuffixList);
}

std::string PublicSuffix::getSuffix(const std::string& url) const {
    struct Match {
        std::string rule;
        size_t level;
        bool exception;
    };
    std::vector<Match> foundRules;

    auto tokens = splitString(url, '.');

    // search for matching rules
    const Node* node = &list;
    size_t level{0};
    for(auto tokenIt = tokens.rbegin(); tokenIt != tokens.rend(); ++tokenIt) {
        ++level;

        // check wildcard rule
        auto wildcard = node->children.find("*");
        if(wildcard != node->children.end()) {
            foundRules.push_back({"*", level, false});
        }

        // check direct rule
        auto child = node->children.find(*tokenIt);
        if(child != node->children.end()) {
            if(child->second->suffix) {
                foundRules.push_back({child->first, level, child->second->exception});
            }

            // advance to the next token
            node = child->second.get();
        } else {
            break;
        }
    }

    if(foundRules.size() > 0) {
        // The prevailing rule is the exception rule or the one with most tokens
        Match* prevailingRule = nullptr;
        for(auto rule : foundRules) {
            if(prevailingRule == nullptr || rule.exception) {
                prevailingRule = &rule;
            } else if(rule.level > prevailingRule->level) {
                prevailingRule = &rule;
            }
        }

        // exception rules describe actual registered domains,
        // so we decrement level to get the public suffix
        if(prevailingRule->exception) {
            --(prevailingRule->level);
        }

        // concatenate level tokens from the back into the public suffix
        size_t firstRule = tokens.size() - prevailingRule->level;
        return std::accumulate(tokens.begin() + firstRule + 1, tokens.end(),
                               tokens[firstRule],
                               [](const std::string& suffix, const std::string& token) {
                                   return suffix + "." + token;
                               });
    } else {
        return "";
    }
}

void PublicSuffix::parseList(std::istream& publicSuffixList) {
    std::string rule;

    while(std::getline(publicSuffixList, rule)) {
        // ignore empty lines and comments
        if(rule.empty() || (rule.size() >= 2 && rule[0] == '/' && rule[1] == '/')) {
            continue;
        }

        // rules end at the first whitespace
        size_t whitespace = rule.find_first_of(" \t");
        if(whitespace != std::string::npos) {
            rule = rule.substr(0, whitespace);
        }

        // exception rules start with a single ! but act the same otherwise
        bool exception{false};
        if(rule[0] == '!') {
            exception = true;
            rule = rule.substr(1);
        }

        auto tokens = splitString(rule, '.');

        // insert tokens into the suffix trie
        Node* node = &list;
        for(auto tokenIt = tokens.rbegin(); tokenIt != tokens.rend(); ++tokenIt) {
            auto child = node->children.find(*tokenIt);

            if(child == node->children.end()) {
                child = node->children.emplace(*tokenIt, std::unique_ptr<Node>(new Node)).first;
            }

            // advance to the next token
            node = child->second.get();
        }

        node->exception = exception;
        node->suffix = true;
    }
}

std::vector<std::string> splitString(const std::string& string, const char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stringStream(string);
    std::string token;

    while(std::getline(stringStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
