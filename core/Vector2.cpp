#include "Vector2.h"

#include <cmath>

std::unique_ptr<Vector2> Vector2::ZERO = std::make_unique<Vector2>(0, 0);

Vector2 Vector2::operator+(const Vector2 &vec) const
{
    return {x + vec.x, y + vec.y};
}

Vector2 Vector2::operator-(const Vector2 &vec) const
{
    return {x - vec.x, y - vec.y};
}

//dot product
double Vector2::operator*(const Vector2 &vec) const
{
    return x * vec.x + y * vec.y;
}

Vector2 Vector2::operator*(double alpha) const
{
    return {alpha * x, alpha * y};
}

void Vector2::operator+=(const Vector2 &vec)
{
    x += vec.x;
    y += vec.y;
}

void Vector2::operator-=(const Vector2 &vec)
{
    x -= vec.x;
    y -= vec.y;
}

void Vector2::operator*=(double alpha)
{
    x *= alpha;
    y *= alpha;
}

bool Vector2::operator==(const Vector2 &vec) const
{
    return x == vec.x && y == vec.y;
}

Vector2 &Vector2::set(double x, double y)
{
    this->x = x;
    this->y = y;

    return *this;
}

Vector2 &Vector2::zero()
{
    x = y = 0;

    return *this;
}

Vector2 &Vector2::normalize()
{
    double norm = len();

    if (norm != 0)
    {
        x /= norm;
        y /= norm;
    }

    return *this;
}

Vector2 &Vector2::flipVertical(double x)
{
    this->x = (x - this->x);
}

Vector2 &Vector2::flipHorizontal(double y)
{
    this->y = (y - this->y);
}

double Vector2::len() const
{
    return std::sqrt(x * x + y * y);
}

double Vector2::dist(const Vector2 &vec) const
{
    return std::sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y));
}

double Vector2::getTheta() const
{
    return std::atan(y / x);
}

double Vector2::getR() const
{
    return len();
}

Vector2::Quadrant Vector2::getQuadrant() const
{
    if (x * y > 0)
    {
        if (x > 0)
            return Quadrant::FIRST;
        else
            return Quadrant::THIRD;
    }
    else
    {
        if (x > 0)
            return Quadrant::FOURTH;
        else
            return Quadrant::SECOND;
    }
}