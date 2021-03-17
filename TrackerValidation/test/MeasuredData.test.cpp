#include "../MeasuredData.h"

#include "catch.hpp"

#include <stdio.h>

TEST_CASE("MeasuredData factory", "[MeasuredData]")
{
    SECTION("Create MeasuredDataStream")
    {
        MeasuredData *data = MeasuredData::create("cout", "label", "tracker", "subject", "path");

        CHECK(data->getLabel() == "label");
        CHECK(data->getTrackerName() == "tracker");
        CHECK(data->getSubject() == "subject");

        delete data;
    }

    SECTION("Create MeasuredDataStream with default path")
    {
        MeasuredData* data = MeasuredData::create("cout", "l", "t", "s");

        CHECK(data->getLabel() == "l");
        CHECK(data->getTrackerName() == "t");
        CHECK(data->getSubject() == "s");

        delete data;
    }

    SECTION("Create MeasuredDataFile")
    {
        char tmpFile[L_tmpnam];
        #ifndef _WIN32
        REQUIRE(tmpnam(tmpFile) == tmpFile); // returns pointer to tmpFile on success
        #else
        REQUIRE(tmpnam_s(tmpFile, sizeof(tmpFile)) == 0);
        #endif

        MeasuredData* data = MeasuredData::create("file", "label", "tracker", "subject", tmpFile);

        CHECK(data->getLabel() == "label");
        CHECK(data->getTrackerName() == "tracker");
        CHECK(data->getSubject() == "subject");

        delete data;
    }

    SECTION("Create MeasuredDataFile with no path")
    {
        REQUIRE_THROWS_AS(MeasuredData::create("file", "l", "t", "s", ""),
                          std::runtime_error);
    }

    SECTION("Create MeasuredDataFile with default path")
    {
        REQUIRE_THROWS_AS(MeasuredData::create("file", "l", "t", "s"),
                          std::runtime_error);
    }

    SECTION("Create MeasuredData with invalid type")
    {
        REQUIRE_THROWS_AS(MeasuredData::create("junk", "l", "t", "s"),
                          std::runtime_error);
    }
}