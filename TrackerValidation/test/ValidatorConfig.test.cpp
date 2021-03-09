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
        REQUIRE(config.targetSize == 6);
        REQUIRE(config.targType == "crosshairbullseye");
        REQUIRE(config.trackerLabel == "Test project");
        REQUIRE(config.tracker == "mouse");
        REQUIRE(config.subject == "Test user");
        REQUIRE(config.outputFile == "");
        REQUIRE(config.trackerConfig.ipAddress == "127.0.0.1");
        REQUIRE(config.trackerConfig.ipPort == 4242);
    }

    SECTION("Other constructor values")
    {
        ValidatorConfig config(1, 2, 3, 4, "a", "b", "c", "d", "e",
                               TrackerConfig("10.0.0.1", 8080));

        REQUIRE(config.cols == 1);
        REQUIRE(config.rows == 2);
        REQUIRE(config.repeats == 3);
        REQUIRE(config.targetSize == 4);
        REQUIRE(config.targType == "a");
        REQUIRE(config.trackerLabel == "b");
        REQUIRE(config.tracker == "c");
        REQUIRE(config.subject == "d");
        REQUIRE(config.outputFile == "e");
        REQUIRE(config.trackerConfig.ipAddress == "10.0.0.1");
        REQUIRE(config.trackerConfig.ipPort == 8080);
    }
}