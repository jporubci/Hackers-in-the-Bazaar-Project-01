#ifndef POINT_HPP
#define POINT_HPP

template <typename T>
struct Point {
    T x, y;

    Point(T _x, T _y) : x(_x), y(_y) {}
};

#endif
