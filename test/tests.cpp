#include <gtest/gtest.h>
#include "plotting/coordinates.h"

TEST(CoordinateMapper, screenToCoordinates){
	/*
	 * | (0,0)
	 * |
	 * |
	 * |____________ (screen_width, screen_height)
	 */
	CoordinateSystemBase screenCoordinates = {
        .xMax = 250,
        .yMax = 125,
		.invertY = true
	};

	/*
	 *(-2, 2)|
	 * ______|______
	 *  (0,0)|
	 *       |     (2,2)
	 */
    CoordinateSystemBase coordinateSystem = {
        .xMin = -2,
		.xMax = 2,
		.yMin = -2,
        .yMax = 2,
    };
	CoordinateMapper mapper = CoordinateMapper(screenCoordinates, coordinateSystem);

	// Center
    ASSERT_EQ(mapper(ml::vec2T<double>{125, 62.5}), ml::vec2T<double>(0, 0));
    // Four corners
    ASSERT_EQ(mapper(ml::vec2T<double>{0.0, 0.0}), ml::vec2T<double>(-2, 2));
    ASSERT_EQ(mapper(ml::vec2T<double>{250, 125}), ml::vec2T<double>(2, -2));
    ASSERT_EQ(mapper(ml::vec2T<double>{0, 125}), ml::vec2T<double>(-2, -2));
    ASSERT_EQ(mapper(ml::vec2T<double>{250, 0}), ml::vec2T<double>(2, 2));


}