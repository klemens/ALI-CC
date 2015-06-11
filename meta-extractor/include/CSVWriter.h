#ifndef _CSVWRITER_H_
#define _CSVWRITER_H_

#include <string>
#include <istream>
#include <stdexcept>

namespace CSV {
    class Writer {
        public:
            static const char CSV_SEP = '|';
            static const char CSV_QUOTE = '"';

            Writer(std::ostream& output);
            Writer(std::ostream& output, const char separator);

            Writer operator<< (const std::string& data);

            Writer next();

        private:
            std::ostream& output;
            char separator;
            bool isFirstValue;

            std::string clean(const std::string&);
    };
}

#endif
