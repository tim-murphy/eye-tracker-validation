#include "../ValidatorConfig.h"

#include "catch.hpp"

TEST_CASE("ValidatorConfig", "[ValidatorConfig]")
{
    SECTION("Default constructor values")
    {
        ValidatorConfig config;

        CHECK(config.cols == 5);
        CHECK(config.rows == 3);
        CHECK(config.repeats == 2);
        CHECK(config.targetSize == 6);
        CHECK(config.targType == "crosshairbullseye");
        CHECK(config.trackerLabel == "Test project");
        CHECK(config.tracker == "mouse");
        CHECK(config.subject == "Test user");
        CHECK(config.outputFile == "");
        CHECK(config.trackerConfig.ipAddress == "127.0.0.1");
        CHECK(config.trackerConfig.ipPort == 0);
    }

    SECTION("Other constructor values")
    {
        ValidatorConfig config(1, 2, 3, 4, "a", "b", "c", "d", "e",
                               TrackerConfig("10.0.0.1", 8080));

        CHECK(config.cols == 1);
        CHECK(config.rows == 2);
        CHECK(config.repeats == 3);
        CHECK(config.targetSize == 4);
        CHECK(config.targType == "a");
        CHECK(config.trackerLabel == "b");
        CHECK(config.tracker == "c");
        CHECK(config.subject == "d");
        CHECK(config.outputFile == "e");
        CHECK(config.trackerConfig.ipAddress == "10.0.0.1");
        CHECK(config.trackerConfig.ipPort == 8080);
    }
}