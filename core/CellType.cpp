#include "CellType.h"

#include <cmath>

bool Color::operator==(const Color &color) const {
    return r == color.r && g == color.g && b == color.b;
}

TypeInfo CellType::SEA(0, Color(0, 255, 255));
TypeInfo CellType::LAND(8760.0, Color(0, 255, 0));
TypeInfo CellType::FRAME(0.0, Color(255, 255, 255));
TypeInfo CellType::EXPOSED_HEADLAND(1.0, Color(255, 0, 0));
TypeInfo CellType::WAVE_CUT_PLATFORM(1.0, Color(21, 134, 22));
TypeInfo CellType::POCKET_BEACH(24.0, Color(255, 0, 144));
TypeInfo CellType::SAND_BEACH(24.0, Color(211, 211, 211));
TypeInfo CellType::SAND_AND_GRAVELBEACH(24.0, Color(255, 192, 203));
TypeInfo CellType::SAND_AND_COBBLE_BEACH(8760.0, Color(255, 255, 0));
TypeInfo CellType::EXPOSED_TIDE_FLATS(1.0, Color(255, 165, 0));
TypeInfo CellType::SHELTERED_ROCK_SHORE(8760.0, Color(220, 220, 220));
TypeInfo CellType::SHELTERED_TIDE_FLAT(8760.0, Color(123, 134, 22));
TypeInfo CellType::SHELTERED_MARSH(8760.0, Color(123, 33, 22));

double TypeInfo::rateConstant() const
{
    return (double)(std::log(2) / (halfTime * 3600));
}

bool TypeInfo::operator==(const TypeInfo &type) const {
    return this->halfTime == type.halfTime && this->color == type.color;
}
