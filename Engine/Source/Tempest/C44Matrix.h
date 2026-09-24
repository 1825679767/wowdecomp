#ifndef ENGINE_SOURCE_TEMPEST_C44MATRIX_H
#define ENGINE_SOURCE_TEMPEST_C44MATRIX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"

class C33Matrix;
class C4Quaternion;

class C44Matrix {
    public:
    static void FromQuaternion(const C4Quaternion& q, C44Matrix& m);
    static C44Matrix Rotation(float angle, const C3Vector& axis, bool normalized);
    static C44Matrix RotationAroundX(float angle);
    static C44Matrix RotationAroundY(float angle);
    static C44Matrix RotationAroundZ(float angle);

    /* 0000 */ float a0;
    /* 0004 */ float a1;
    /* 0008 */ float a2;
    /* 000C */ float a3;
    /* 0010 */ float b0;
    /* 0014 */ float b1;
    /* 0018 */ float b2;
    /* 001C */ float b3;
    /* 0020 */ float c0;
    /* 0024 */ float c1;
    /* 0028 */ float c2;
    /* 002C */ float c3;
    /* 0030 */ float d0;
    /* 0034 */ float d1;
    /* 0038 */ float d2;
    /* 003C */ float d3;

    C44Matrix();
    C44Matrix(const C33Matrix& m);
    C44Matrix(const C4Quaternion& q);
    C44Matrix Adjoint() const;
    C44Matrix AffineInverse() const;
    C44Matrix AffineInverse(float scale) const;
    float Determinant() const;
    C44Matrix Inverse() const;
    C44Matrix Inverse(float determinant) const;
    void Rotate(const C4Quaternion& q);
    void RotateAroundX(float angle);
    void RotateAroundY(float angle);
    void RotateAroundZ(float angle);
    void Scale(const C3Vector& scale);
    void Scale(float scale);
    void Translate(const C3Vector& move);
    C44Matrix Transpose() const;
    C44Matrix& operator*=(const C44Matrix& m);
    C44Matrix& operator*=(float s);
    C44Matrix& operator=(const C44Matrix& m);
};

bool operator!=(const C44Matrix& l, const C44Matrix& r);
C44Matrix operator*(const C44Matrix& l, const C44Matrix& r);
C44Matrix operator*(const C44Matrix& l, float s);
C3Vector operator*(const C3Vector& v, const C44Matrix& m);
C4Vector operator*(const C4Vector& v, const C44Matrix& m);
C3Vector operator*=(C3Vector& v, const C44Matrix& m);
C44Matrix operator+(const C44Matrix& l, const C44Matrix& r);
C3Vector TransformDirection(const C3Vector& v, const C44Matrix& m);

#endif
