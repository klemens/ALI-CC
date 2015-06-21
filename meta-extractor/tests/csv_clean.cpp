#include "catch.hpp"

#include <sstream>
#include "CSVWriter.h"

class CSVWriterTest : public CSV::Writer {
    public:
        CSVWriterTest(std::ostream& output, const char separator) : CSV::Writer(output, separator) {}

        std::string clean(const std::string& data) {
            return CSV::Writer::clean(data);
        }
};

TEST_CASE("Test CSV::Writer::clean escaping", "[csv]") {
    std::ostringstream foo;

    SECTION("using , as separator") {
        CSVWriterTest writer(foo, ',');

        REQUIRE(writer.clean("") == "");
        REQUIRE(writer.clean("a") == "a");
        REQUIRE(writer.clean("abcxyz22") == "abcxyz22");
        REQUIRE(writer.clean("a,b") == "\"a,b\"");
        // a,"b" -> "a,""b"""
        REQUIRE(writer.clean("a,\"b\"") == "\"a,\"\"b\"\"\"");
        // " without separator does not need to be escaped
        REQUIRE(writer.clean("\"") == "\"");
        REQUIRE(writer.clean("nice \"test\" case") == "nice \"test\" case");
        REQUIRE(writer.clean("nice, \"test\" case") == "\"nice, \"\"test\"\" case\"");
    }
}
