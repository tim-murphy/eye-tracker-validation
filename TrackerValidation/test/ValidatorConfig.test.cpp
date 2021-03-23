#include "../ValidatorConfig.h"

#include "catch.hpp"

TEST_CASE("ValidatorConfig", "[ValidatorConfig]")
{
    SECTION("Default constructor values")
    {
        ValidatorConfig config;

        REQUIRE(config.cols == 5);
        REQUIRE(config.rows == 3);
        REQUIRE(config.repeats == 2);
        CHECK(config.padding == 0);
        REQUIRE(config.targetSize == 6);
        REQUIRE(config.targType == "crosshairbullseye");
        CHECK(config.targLocation == "middle");
        REQUIRE(config.trackerLabel == "Test project");
        REQUIRE(config.tracker == "mouse");
        REQUIRE(config.subject == "Test user");
        REQUIRE(config.outputFile == "");
        CHECK(config.preview == false);
        REQUIRE(config.trackerConfig.ipAddress == "127.0.0.1");
        REQUIRE(config.trackerConfig.ipPort == 4242);
    }

    SECTION("Other constructor values")
    {
        ValidatorConfig config(1, 2, 3, 100, 4, "a", "corners", "b", "c", "d", "e", true,
                               TrackerConfig("10.0.0.1", 8080));

        REQUIRE(config.cols == 1);
        REQUIRE(config.rows == 2);
        REQUIRE(config.repeats == 3);
        CHECK(config.padding == 100);
        REQUIRE(config.targetSize == 4);
        REQUIRE(config.targType == "a");
        CHECK(config.targLocation == "corners");
        REQUIRE(config.trackerLabel == "b");
        REQUIRE(config.tracker == "c");
        REQUIRE(config.subject == "d");
        REQUIRE(config.outputFile == "e");
        CHECK(config.preview == true);
        REQUIRE(config.trackerConfig.ipAddress == "10.0.0.1");
        REQUIRE(config.trackerConfig.ipPort == 8080);
    }
}