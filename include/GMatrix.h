/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GMatrix_DEFINED
#define GMatrix_DEFINED

#include "GColor.h"
#include "GMath.h"
#include "GPoint.h"
#include "GRect.h"
#include <optional>

class GMatrix {
public:
    /** [ a  c  e ]         [ 0 2 4 ] <-- indices
     *  [ b  d  f ]         [ 1 3 5 ]
     *  [ 0  0  1 ]  <-- implied, not stored
     */
    GMatrix(float a, float c, float e, float b, float d, float f) {
        fMat[0] = a;    fMat[2] = c;    fMat[4] = e;
        fMat[1] = b;    fMat[3] = d;    fMat[5] = f;
    }

    GMatrix(GVector e0, GVector e1, GVector origin) {
        fMat[0] = e0.x;    fMat[2] = e1.x;    fMat[4] = origin.x;
        fMat[1] = e0.y;    fMat[3] = e1.y;    fMat[5] = origin.y;
    }

    GMatrix(const GMatrix& other) = default;

    GVector e0() const { return {fMat[0], fMat[1]}; }
    GVector e1() const { return {fMat[2], fMat[3]}; }
    GVector origin() const { return {fMat[4], fMat[5]}; }

    float operator[](int index) const {
        assert(index >= 0 && index < 6);
        return fMat[index];
    }
    float& operator[](int index) {
        assert(index >= 0 && index < 6);
        return fMat[index];
    }

    bool operator==(const GMatrix& m) {
        for (int i = 0; i < 6; ++i) {
            if (fMat[i] != m.fMat[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const GMatrix& m) { return !(*this == m); }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // These methods must be implemented by the student.

    GMatrix();  // initialize to identity
    static GMatrix Translate(float tx, float ty);
    static GMatrix Scale(float sx, float sy);
    static GMatrix Rotate(float radians);

    /**
     *  Return the product of two matrices: a * b
     */
    static GMatrix Concat(const GMatrix& a, const GMatrix& b);

    /*
     *  If the inverse exists, return it, else return {} to signal no return value.
     */
    std::optional<GMatrix> invert() const;

    /**
     *  Transform the set of points in src, storing the resulting points in dst, by applying this
     *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
     *
     *  [ a  c  e ] [ x ]     x' = ax + cy + e
     *  [ b  d  f ] [ y ]     y' = bx + dy + f
     *  [ 0  0  1 ] [ 1 ]
     *
     *  Note: It is legal for src and dst to point to the same memory (however, they may not
     *  partially overlap). Thus the following is supported.
     *
     *  GPoint pts[] = { ... };
     *  matrix.mapPoints(pts, pts, count);
     */
    void mapPoints(GPoint dst[], const GPoint src[], int count) const;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // These helper methods are implemented in terms of the previous methods.

    friend GMatrix operator*(const GMatrix& a, const GMatrix& b) {
        return Concat(a, b);
    }

    void mapPoints(GPoint pts[], int count) const {
        this->mapPoints(pts, pts, count);
    }

    GPoint operator*(GPoint p) const {
        this->mapPoints(&p, 1);
        return p;
    }

private:
    float fMat[6];
};

#endif
