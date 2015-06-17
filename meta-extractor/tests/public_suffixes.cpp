#include "catch.hpp"

#include <fstream>
#include "PublicSuffix.h"

TEST_CASE("Test PublicSuffix") {
    std::string dir = "tests/resources/";

    SECTION("public_suffix_list.dat") {
        std::ifstream input(dir + "public_suffix_list.dat");
        REQUIRE(input.good());

        PublicSuffix suffixes(input);

        // invalid / non-existent domains
        REQUIRE(suffixes.getSuffix("") == "");
        REQUIRE(suffixes.getSuffix("test") == "");
        REQUIRE(suffixes.getSuffix("local") == "");

        // simple tlds with one rule
        REQUIRE(suffixes.getSuffix("de") == "de");
        REQUIRE(suffixes.getSuffix("example.de") == "de");
        REQUIRE(suffixes.getSuffix("www.example.de") == "de");

        // tlds with multiple simple rules
        REQUIRE(suffixes.getSuffix("co") == "co");
        REQUIRE(suffixes.getSuffix("example.co") == "co");
        REQUIRE(suffixes.getSuffix("www.example.co") == "co");
        REQUIRE(suffixes.getSuffix("com.co") == "com.co");
        REQUIRE(suffixes.getSuffix("example.com.co") == "com.co");
        REQUIRE(suffixes.getSuffix("www.example.com.co") == "com.co");

        // tld with one second level wildcard rule
        REQUIRE(suffixes.getSuffix("il") == "");
        REQUIRE(suffixes.getSuffix("example.il") == "example.il");
        REQUIRE(suffixes.getSuffix("www.example.il") == "example.il");

        // tld with second level wildcard and exception
        REQUIRE(suffixes.getSuffix("mz") == "");
        REQUIRE(suffixes.getSuffix("example.mz") == "example.mz");
        REQUIRE(suffixes.getSuffix("www.example.mz") == "example.mz");
        REQUIRE(suffixes.getSuffix("teledata.mz") == "mz");

        // complicated tld with multiple rules, wildcards and exceptions
        REQUIRE(suffixes.getSuffix("jp") == "jp");
        REQUIRE(suffixes.getSuffix("example.jp") == "jp");
        REQUIRE(suffixes.getSuffix("www.example.jp") == "jp");
        REQUIRE(suffixes.getSuffix("co.jp") == "co.jp");
        REQUIRE(suffixes.getSuffix("example.co.jp") == "co.jp");
        REQUIRE(suffixes.getSuffix("www.example.co.jp") == "co.jp");
        REQUIRE(suffixes.getSuffix("yokohama.jp") == "jp");
        REQUIRE(suffixes.getSuffix("example.yokohama.jp") == "example.yokohama.jp");
        REQUIRE(suffixes.getSuffix("www.example.yokohama.jp") == "example.yokohama.jp");
        REQUIRE(suffixes.getSuffix("city.yokohama.jp") == "yokohama.jp");
        REQUIRE(suffixes.getSuffix("example.aichi.jp") == "aichi.jp");
        REQUIRE(suffixes.getSuffix("aisai.aichi.jp") == "aisai.aichi.jp");
        REQUIRE(suffixes.getSuffix("example.aisai.aichi.jp") == "aisai.aichi.jp");

        // non ascii chars
        REQUIRE(suffixes.getSuffix("vermögensberater") == "vermögensberater");
        REQUIRE(suffixes.getSuffix("example.vermögensberater") == "vermögensberater");
        REQUIRE(suffixes.getSuffix("中国") == "中国");
        REQUIRE(suffixes.getSuffix("example.中国") == "中国");
        REQUIRE(suffixes.getSuffix("ελ") == "ελ");
        REQUIRE(suffixes.getSuffix("example.ελ") == "ελ");
        REQUIRE(suffixes.getSuffix("公司.cn") == "公司.cn");
        REQUIRE(suffixes.getSuffix("example.公司.cn") == "公司.cn");

        // private domains
        REQUIRE(suffixes.getSuffix("cloudfront.net") == "cloudfront.net");
        REQUIRE(suffixes.getSuffix("example.cloudfront.net") == "cloudfront.net");
        REQUIRE(suffixes.getSuffix("eu-central-1.compute.amazonaws.com") == "eu-central-1.compute.amazonaws.com");
        REQUIRE(suffixes.getSuffix("example.eu-central-1.compute.amazonaws.com") == "eu-central-1.compute.amazonaws.com");
        REQUIRE(suffixes.getSuffix("fastly.net") == "net");
        REQUIRE(suffixes.getSuffix("ssl.fastly.net") == "net");
        REQUIRE(suffixes.getSuffix("b.ssl.fastly.net") == "b.ssl.fastly.net");
        REQUIRE(suffixes.getSuffix("example.b.ssl.fastly.net") == "b.ssl.fastly.net");
        REQUIRE(suffixes.getSuffix("github.io") == "github.io");
        REQUIRE(suffixes.getSuffix("example.github.io") == "github.io");
    }
}
