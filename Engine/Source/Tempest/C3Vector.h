#ifndef ENGINE_SOURCE_TEMPEST_C3VECTOR_H
#define ENGINE_SOURCE_TEMPEST_C3VECTOR_H

#include <cstddef>
#include <cstdint>

class C4Vector;
class CImVector;

class C3Vector {
    public:
    enum EAxis {
        AXIS_X = 0,
        AXIS_Y = 1,
        AXIS_Z = 2
    };

    static C3Vector Cross(const C3Vector& a, const C3Vector& b);
    static C3Vector Max(const C3Vector& a, const C3Vector& b);
    static C3Vector Min(const C3Vector& a, const C3Vector& b);

    /* 0000 */ float x;
    /* 0004 */ float y;
    /* 0008 */ float z;

    C3Vector() = default;
    C3Vector(const C4Vector& v);
    C3Vector(const CImVector& color);
    EAxis MajorAxis() const;
    EAxis MinorAxis() const;
    void Normalize();
    void NormalizeSafe();
    void Zero();
    C3Vector operator-() const;
    C3Vector& operator+=(const C3Vector& v);
    C3Vector& operator*=(float s);
    C3Vector& operator/=(float s);
};

C3Vector operator+(const C3Vector& l, const C3Vector& r);
C3Vector operator-(const C3Vector& l, const C3Vector& r);
bool operator==(const C3Vector& l, const C3Vector& r);
bool operator!=(const C3Vector& l, const C3Vector& r);

#endif
