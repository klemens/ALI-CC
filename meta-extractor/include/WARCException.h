#ifndef _WARCEXCEPTION_H_
#define _WARCEXCEPTION_H_

#include <stdexcept>

namespace WARC {
    class Exception : public std::runtime_error {
        public:
            Exception(const std::string& error) : std::runtime_error(error) {}
    };
}

#endif
