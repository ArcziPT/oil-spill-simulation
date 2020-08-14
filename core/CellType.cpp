#include "CellType.h"

#include <cmath>

static TypeInfo SEA(0, Color(0, 255, 255));
static TypeInfo LAND(8760.0, Color(0, 255, 0));
static TypeInfo FRAME(0.0, Color(255, 255, 255));
static TypeInfo EXPOSED_HEADLAND(1.0, Color(255, 0, 0));
static TypeInfo WAVE_CUT_PLATFORM(1.0, Color(21, 134, 22));
static TypeInfo POCKET_BEACH(24.0, Color(255, 0, 144));
static TypeInfo SAND_BEACH(24.0, Color(211, 211, 211));
static TypeInfo SAND_AND_GRAVELBEACH(24.0, Color(255, 192, 203));
static TypeInfo SAND_AND_COBBLE_BEACH(8760.0, Color(255, 255, 0));
static TypeInfo EXPOSED_TIDE_FLATS(1.0, Color(255, 165, 0));
static TypeInfo SHELTERED_ROCK_SHORE(8760.0, Color(220, 220, 220));
static TypeInfo SHELTERED_TIDE_FLAT(8760.0, Color(123, 134, 22));
static TypeInfo SHELTERED_MARSH(8760.0, Color(123, 33, 22));

double TypeInfo::rateConstant()
{
    return (double)(std::log(2) / (halfTime * 3600));
}