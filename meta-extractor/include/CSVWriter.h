#ifndef _CSVWRITER_H_
#define _CSVWRITER_H_

#include <string>
#include <istream>
#include <stdexcept>

namespace CSV {
    class Writer {
        public:
            /**
             * Construct a CSV::Writer that uses ',' as value separator.
             */
            Writer(std::ostream& output);
            /**
             * Construct a CSV::Writer that uses the given separator.
             */
            Writer(std::ostream& output, const char separator);

            /**
             * Write the given value to the csv file
             */
            Writer operator<< (const std::string& data);
            /**
             * Advance to the next line
             */
            Writer next();

        protected:
            /**
             * Clean up (escape) the given value to be used in this object.
             * Note: This is called internally in operator<<(const std::string&)
             */
            std::string clean(const std::string& data);

        private:
            std::ostream& output;
            char separator;
            bool isFirstValue{true};
    };
}

#endif
