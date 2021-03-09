#include "../ScreenPositionStore.h"

// note: this needs to go after our project code in order to pick up overloaded
//       operator<< methods. Otherwise we will get abstract {?} == {?} messages
//       when comparisons fail.
#include "catch.hpp"

TEST_CASE("ScreenPositionStore", "[ScreenPositionStore]")
{
    ScreenPositionStore store;

    SECTION("Default constructor values")
    {
        REQUIRE(store.getIdentifier() == 0.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>());
    }

    // test getCurrentPosition and getIdentifier while we're here
    SECTION("setCurrentPosition")
    {
        store.setCurrentPosition(std::make_pair(100, 200), -1.0);
        REQUIRE(store.getIdentifier() == -1.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>(100, 200));

        store.setCurrentPosition(std::make_pair(2, 1), 1.0);
        REQUIRE(store.getIdentifier() == 1.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>(2, 1));

        store.setCurrentPosition(std::make_pair(99, 99), 0.0);
        REQUIRE(store.getIdentifier() == 0.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>(99, 99));

        // we only take positive values. If we feed in a negative value, it's going to wrap
        store.setCurrentPosition(std::make_pair(-1, -2), 0.0);
        REQUIRE(store.getIdentifier() == 0.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>(0xffffffff, 0xffffffff - 1));
    }

    // edge case - default value fot setCurrentPosition
    SECTION("getIdentifier")
    {
        store.setCurrentPosition(std::make_pair(100, 200));
        REQUIRE(store.getIdentifier() == 0.0);
        REQUIRE(store.getCurrentPosition() == std::pair<unsigned int, unsigned int>(100, 200));
    }
}
