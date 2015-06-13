#ifndef _WARCEXCEPTION_H_
#define _WARCEXCEPTION_H_

#include <stdexcept>

namespace WARC {
    class Exception : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

#endif
