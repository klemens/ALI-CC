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

    SECTION("Value::extractTld") {
        REQUIRE(Value::extractTld("domain.test") == "test");
        REQUIRE(Value::extractTld("so.many.domains.test") == "test");
        REQUIRE(Value::extractTld(".test") == "test");
        REQUIRE(Value::extractTld("test.") == "");
        REQUIRE(Value::extractTld("test") == "");
        REQUIRE(Value::extractTld("") == "");
    }

    SECTION("Value::extractMIME") {
        REQUIRE(Value::extractMIME("text/html") == "text/html");
        REQUIRE(Value::extractMIME("text/http; charset=UTF-8") == "text/http");
        REQUIRE(Value::extractMIME("text/html;") == "text/html");
        REQUIRE(Value::extractMIME("text/html;;;;;") == "text/html");
        REQUIRE(Value::extractMIME(";text/html;;;;;") == "");
        REQUIRE(Value::extractMIME("") == "");
    }

    SECTION("Value::extractCharset") {
        REQUIRE(Value::extractCharset("") == "");
        REQUIRE(Value::extractCharset("text/html") == "");
        REQUIRE(Value::extractCharset("text/html; charset=UTF-8") == "utf-8");
        REQUIRE(Value::extractCharset("text/html; charset=UTF-8 ") == "utf-8");
        REQUIRE(Value::extractCharset("text/html; charset=UTF-8; peter") == "utf-8");
        REQUIRE(Value::extractCharset("application/xml; charset=iso-8859-15") == "iso-8859-15");
        REQUIRE(Value::extractCharset("text/html; charset=UTF8") == "utf8");
    }

    SECTION("Value::canonicalizeCharset") {
        REQUIRE(Value::canonicalizeCharset("") == "");
        REQUIRE(Value::canonicalizeCharset("utf-8") == "utf");
        REQUIRE(Value::canonicalizeCharset("utf8") == "utf");
        REQUIRE(Value::canonicalizeCharset("utf-16") == "utf");
        REQUIRE(Value::canonicalizeCharset("iso-8859-15") == "iso-8859");
        REQUIRE(Value::canonicalizeCharset("shift_jis") == "jp");
        REQUIRE(Value::canonicalizeCharset("iso-2022-jp") == "jp");
        REQUIRE(Value::canonicalizeCharset("big5") == "cn");
        REQUIRE(Value::canonicalizeCharset("gbk") == "cn");
        REQUIRE(Value::canonicalizeCharset("euc-kr") == "kr");
        REQUIRE(Value::canonicalizeCharset("macintosh") == "other");
        REQUIRE(Value::canonicalizeCharset("ibm866") == "other");
    }

    SECTION("Value::canonicalizeServer") {
        REQUIRE(Value::canonicalizeServer("") == "");
        REQUIRE(Value::canonicalizeServer("webserver") == "other");
        REQUIRE(Value::canonicalizeServer("Nginx/1.5.6 (Ubuntu)") == "nginx");
        REQUIRE(Value::canonicalizeServer("cloudflare-nginx") == "nginx");
        REQUIRE(Value::canonicalizeServer("Microsoft-IIS/8.5") == "iis");
        REQUIRE(Value::canonicalizeServer("lighttpd/2.0.0") == "lighttpd");
        REQUIRE(Value::canonicalizeServer("Jetty(8.1.13.v20130916)") == "jetty");
        REQUIRE(Value::canonicalizeServer("GSE") == "gse");
        REQUIRE(Value::canonicalizeServer("Apache/2.2.27 (Unix) PHP/5.3.5") == "apache");
        REQUIRE(Value::canonicalizeServer("Apache-Coyote/1.1") == "apache-coyote");
        REQUIRE(Value::canonicalizeServer("hiawatha 9.0") == "hiawatha");
    }

    SECTION("Value::isIPv6") {
        REQUIRE(Value::isIPv6("") == false);
        REQUIRE(Value::isIPv6("127.0.0.1") == false);
        REQUIRE(Value::isIPv6("127.0.0.1:80") == false);
        REQUIRE(Value::isIPv6("::1") == true);
        REQUIRE(Value::isIPv6("2001:a34::80") == true);
        REQUIRE(Value::isIPv6("[2001:a34::80]:80") == true);
    }

    SECTION("Value::usesCompression") {
        REQUIRE(Value::usesCompression("") == false);
        REQUIRE(Value::usesCompression("gzip") == true);
        REQUIRE(Value::usesCompression("deflate") == true);
        REQUIRE(Value::usesCompression("identity") == false);
    }

    SECTION("Value::prefix") {
        REQUIRE(Value::prefix("", "") == true);
        REQUIRE(Value::prefix("abc", "") == true);
        REQUIRE(Value::prefix("abc", "a") == true);
        REQUIRE(Value::prefix("abc", "ab") == true);
        REQUIRE(Value::prefix("abc", "abc") == true);
        REQUIRE(Value::prefix("abc", "abcd") == false);
        REQUIRE(Value::prefix("bcd", "a") == false);
        REQUIRE(Value::prefix("bcd", "ab") == false);
        REQUIRE(Value::prefix("bcd", "abc") == false);
        REQUIRE(Value::prefix("bcd", "abcd") == false);
        REQUIRE(Value::prefix("", "a") == false);
    }

    SECTION("Value::equalsI") {
        REQUIRE(Value::equalsI("", "") == true);
        REQUIRE(Value::equalsI("a", "") == false);
        REQUIRE(Value::equalsI("", "a") == false);
        REQUIRE(Value::equalsI("a", "a") == true);
        REQUIRE(Value::equalsI("A", "a") == true);
        REQUIRE(Value::equalsI("a", "A") == true);
        REQUIRE(Value::equalsI("A", "A") == true);
        REQUIRE(Value::equalsI("aBc", "AbC") == true);
    }
}
