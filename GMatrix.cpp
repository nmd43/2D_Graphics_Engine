/*
*  Copyright 2024 <me>
*/

#include "include/GMatrix.h"
#include "include/GPoint.h"
#include <optional>

GMatrix::GMatrix() {
    fMat[0] = 1.0f; fMat[2] = 0.0f; fMat[4] = 0.0f;
    fMat[1] = 0.0f; fMat[3] = 1.0f; fMat[5] = 0.0f;
}


GMatrix GMatrix::Translate(float tx, float ty){
    return GMatrix(1.0f, 0.0f, tx, 0.0f, 1.0f, ty);
}

GMatrix GMatrix::Scale(float sx, float sy){
    return GMatrix(sx, 0.0f, 0.0f, 0.0f, sy, 0.0f);
}

GMatrix GMatrix::Rotate(float radians){
        return GMatrix(cos(radians), -sin(radians), 0.0f, sin(radians), cos(radians), 0.0f);
}

GMatrix GMatrix::Concat(const GMatrix& secundo, const GMatrix& primo){
    float zero = secundo[0]*primo[0] + secundo[2]*primo[1];
    float one = secundo[1]*primo[0] + secundo[3]*primo[1];
    float two = secundo[0]*primo[2] + secundo[2]*primo[3];
    float three = secundo[1]*primo[2] + secundo[3]*primo[3];
    float four = secundo[0]*primo[4] + secundo[2]*primo[5] + secundo[4];
    float five = secundo[1]*primo[4] + secundo[3]*primo[5] + secundo[5];
    return GMatrix(zero, two, four, one, three, five);
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const{
    for (int i =0; i<count; i++){
        GPoint point = src[i];
        float x = point.x;
        float y = point.y;

        float newX = this->fMat[0] * x + this->fMat[2] * y + this->fMat[4];
        float newY = this->fMat[1] * x + this->fMat[3] * y + this->fMat[5];
        dst[i] = { newX, newY };
    }
}

std::optional<GMatrix> GMatrix::invert() const {
    float a = this->fMat[0];
    float b = this->fMat[1];
    float c = this->fMat[2];
    float d = this->fMat[3];
    float e = this->fMat[4];
    float f = this->fMat[5];

    float determinant = a * d - b * c;
    if (determinant == 0) {
        return std::nullopt;
    }

    float divisor = 1 / determinant;

    GMatrix invertedMatrix(
         d * divisor, -c * divisor, (c * f - d * e) * divisor,
        -b * divisor, a * divisor, (e * b - a * f) * divisor);

    return invertedMatrix;
}
