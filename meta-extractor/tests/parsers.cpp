#include "catch.hpp"

#include "ValueParsers.h"

TEST_CASE("Test ValueParsers", "[warc]") {
    SECTION("Value::parseId") {
        REQUIRE(Value::parseId("<urn:uuid:00000000-0000-0000-0000-000000000000>")
                == "00000000000000000000000000000000");
        REQUIRE(Value::parseId("<urn:uuid:54231d72-1d15-411b-b282-3f21b8d0ce2f>")
                == "54231d721d15411bb2823f21b8d0ce2f");
        REQUIRE(Value::parseId("<urn:uuid:8a3c71e7-49e3-42dd-9d0c-f438e5458089>")
                == "8a3c71e749e342dd9d0cf438e5458089");

        REQUIRE_THROWS(Value::parseId(""));
        REQUIRE_THROWS(Value::parseId("54231d721d15411bb2823f21b8d0ce2f"));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:12345>"));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:54231d72-1d15411b-b282-3f21b8d0ce2f>"));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:54231d72-1d15-411b-b2823-f21b8d0ce2f>"));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:54231d72-1d15-411b-b282-3f21b-8d0ce2f>"));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:54231d72-1d15-411b-b282-3f21b8d0ce2f> "));
        REQUIRE_THROWS(Value::parseId("<urn:uuid:54231d72-1d15-411b-b282-3f218d0ce2f>"));
        // note the g at the beginning
        REQUIRE_THROWS(Value::parseId("<urn:uuid:g4231d72-1d15-411b-b282-3f21b8d0ce2f>"));
    }
}
