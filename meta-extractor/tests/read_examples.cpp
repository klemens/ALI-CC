#include "catch.hpp"

#include <fstream>
#include "WARCReader.h"
#include "WARCRecord.h"

TEST_CASE("Read example files", "[warc]") {
    std::string dir = "tests/resources/";

    SECTION("request-1.warc") {
        std::ifstream input(dir + "request-1.warc", std::ifstream::binary);
        REQUIRE(input.good());

        WARC::Reader reader(input);
        WARC::Record<void> record1;

        REQUIRE(reader.read(record1) == true);
        REQUIRE(record1.type == "request");
        REQUIRE(record1.id == "<urn:uuid:4885803b-eebd-4b27-a090-144450c11594>");
        REQUIRE(record1.date == "2006-09-19T17:20:24Z");
        REQUIRE(record1.length == 238u);

        WARC::Record<void> record2;
        REQUIRE(reader.read(record2) == true);
        REQUIRE(record2.type == "metadata");
        REQUIRE(record2.id == "<urn:uuid:16da6da0-bcdc-49c3-927e-57494593b943>");
        REQUIRE(record2.date == "2006-09-19T19:20:24Z");
        REQUIRE(record2.length == 63u);

        WARC::Record<void> record3;
        REQUIRE(reader.read(record3) == false);
    }
}
