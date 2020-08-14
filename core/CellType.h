#ifndef CELL_TYPE_H
#define CELL_TYPE_H

struct Color
{
    int r;
    int g;
    int b;

    Color(int r, int g, int b) : r(r), g(g), b(b) {}
};

struct TypeInfo
{
    double halfTime;
    Color color;

    TypeInfo(double halfTime, Color color) : halfTime(halfTime), color(color) {}
    double rateConstant();
};

struct CellType
{
    static TypeInfo SEA;
    static TypeInfo LAND;
    static TypeInfo FRAME;
    static TypeInfo EXPOSED_HEADLAND;
    static TypeInfo WAVE_CUT_PLATFORM;
    static TypeInfo POCKET_BEACH;
    static TypeInfo SAND_BEACH;
    static TypeInfo SAND_AND_GRAVELBEACH;
    static TypeInfo SAND_AND_COBBLE_BEACH;
    static TypeInfo EXPOSED_TIDE_FLATS;
    static TypeInfo SHELTERED_ROCK_SHORE;
    static TypeInfo SHELTERED_TIDE_FLAT;
    static TypeInfo SHELTERED_MARSH;
};

#endif