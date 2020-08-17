#include "CellType.h"

#include <cmath>

static TypeInfo SEA(1, 0, Color(0, 255, 255));
static TypeInfo LAND(2, 8760.0, Color(0, 255, 0));
static TypeInfo FRAME(3, 0.0, Color(255, 255, 255));
static TypeInfo EXPOSED_HEADLAND(4, 1.0, Color(255, 0, 0));
static TypeInfo WAVE_CUT_PLATFORM(5, 1.0, Color(21, 134, 22));
static TypeInfo POCKET_BEACH(6, 24.0, Color(255, 0, 144));
static TypeInfo SAND_BEACH(7, 24.0, Color(211, 211, 211));
static TypeInfo SAND_AND_GRAVELBEACH(8, 24.0, Color(255, 192, 203));
static TypeInfo SAND_AND_COBBLE_BEACH(9, 8760.0, Color(255, 255, 0));
static TypeInfo EXPOSED_TIDE_FLATS(10, 1.0, Color(255, 165, 0));
static TypeInfo SHELTERED_ROCK_SHORE(11, 8760.0, Color(220, 220, 220));
static TypeInfo SHELTERED_TIDE_FLAT(12, 8760.0, Color(123, 134, 22));
static TypeInfo SHELTERED_MARSH(13, 8760.0, Color(123, 33, 22));

double TypeInfo::rateConstant()
{
    return (double)(std::log(2) / (halfTime * 3600));
}

bool TypeInfo::operator==(const TypeInfo &type) const {
    return this->id == type.id;
}
