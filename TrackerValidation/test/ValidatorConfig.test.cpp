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
        CHECK(config.padding == 0);
        CHECK(config.targetSize == 6);
        CHECK(config.targType == "crosshairbullseye");
        CHECK(config.targLocation == "middle");
        CHECK(config.trackerLabel == "Test project");
        CHECK(config.tracker == "mouse");
        CHECK(config.subject == "Test user");
        CHECK(config.outputFile == "");
        CHECK(config.preview == false);
        CHECK(config.trackerConfig.ipAddress == "127.0.0.1");
        CHECK(config.trackerConfig.ipPort == 4242);
    }

    SECTION("Other constructor values")
    {
        ValidatorConfig config(1, 2, 3, 100, 4, "a", "corners", "b", "c", "d", "e", true,
                               TrackerConfig("10.0.0.1", 8080));

        CHECK(config.cols == 1);
        CHECK(config.rows == 2);
        CHECK(config.repeats == 3);
        CHECK(config.padding == 100);
        CHECK(config.targetSize == 4);
        CHECK(config.targType == "a");
        CHECK(config.targLocation == "corners");
        CHECK(config.trackerLabel == "b");
        CHECK(config.tracker == "c");
        CHECK(config.subject == "d");
        CHECK(config.outputFile == "e");
        CHECK(config.preview == true);
        CHECK(config.trackerConfig.ipAddress == "10.0.0.1");
        CHECK(config.trackerConfig.ipPort == 8080);
    }
}