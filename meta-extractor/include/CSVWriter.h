#ifndef _CSVWRITER_H_
#define _CSVWRITER_H_

#include <string>
#include <istream>
#include <stdexcept>

namespace CSV {
    class Writer {
        public:
            static const char CSV_SEP = '|';

            Writer(std::ostream& output);

            Writer operator<< (const std::string& data);

            Writer next();

        private:
            std::ostream& output;
            bool isFirstValue;

            std::string clean(const std::string&);
    };
}

#endif
