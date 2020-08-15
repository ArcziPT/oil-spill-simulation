#ifndef VECTOR2_H
#define VECTOR2_H

#include <memory>

class Vector2
{

public:
    //const
    Vector2() = default;

    Vector2(double x, double y) : x(x), y(y) {}
    Vector2(const Vector2 &vec) : x(vec.x), y(vec.y) {}
    Vector2(const std::unique_ptr<Vector2> &vec) : x(vec->x), y(vec->y) {}

    //special
    static std::unique_ptr<Vector2> ZERO;

    //TODO: Assigments and copy/move constructors

    //Math
    Vector2 operator+(const Vector2 &vec) const;
    Vector2 operator-(const Vector2 &vec) const;
    double operator*(const Vector2 &vec) const;
    Vector2 operator*(double alpha) const;
    void operator+=(const Vector2 &vec);
    void operator-=(const Vector2 &vec);
    void operator*=(double alpha);
    bool operator==(const Vector2 &vec) const;

    Vector2 &set(double x, double y);
    Vector2 &zero();
    Vector2 &normalize();
    Vector2 &flipVertical(double x);
    Vector2 &flipHorizontal(double y);
    double len() const;
    double dist(const Vector2 &vec) const;

    double getTheta() const;
    double getR() const;

    //Quarter
    enum class Quadrant
    {
        FIRST,
        SECOND,
        THIRD,
        FOURTH
    };

    Quadrant getQuadrant() const;

private:
    double x;
    double y;
};

#endif