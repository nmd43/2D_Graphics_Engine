/*
 *  Copyright 2024 <me>
 */


#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include <optional>

class BitmapShader : public GShader {
public:

    BitmapShader(const GBitmap& bitmap, const GMatrix& matrix, GTileMode mode): localMatrix(matrix), fDevice(bitmap) {
        tileMode = mode;

        //Calculate inverse weight and height
        width_inv = 1.0f/(float) bitmap.width();
        height_inv = 1.0f/(float) bitmap.height();
    }

    bool isOpaque() override {
        return fDevice.isOpaque();
    }

    bool setContext (const GMatrix& ctm) override {

        //set inverseMatrix as inverse(ctm * localMatrix)

        GMatrix temp = GMatrix::Concat(ctm, localMatrix);

        std::optional<GMatrix> invertedCtm = temp.invert();

        if (!(invertedCtm).has_value()){
            return false;
        }
        
        inverseMatrix = *invertedCtm;
        return true;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        // Map the given point (x+0.5, y+0.5) using the inverse matrix
        GPoint points[1] {x+0.5f, y+0.5f};
        inverseMatrix.mapPoints(points, points, 1);
        GPoint temp = points[0];
        int newX, newY;

        for (int i=0; i<count; i++){
            
            // Any pixel outside the bitmap should be filled with the color of the nearest edge pixel.
            if (tileMode == GTileMode::kClamp) {
                newX = GFloorToInt(temp.x);
                newY = GFloorToInt(temp.y);
                //Ensure points are within the dimensions of the bitmap
                newX = std::max(0, std::min(fDevice.width() - 1, newX));
                newY = std::max(0, std::min(fDevice.height() - 1, newY));
            }

            // Any coordinate outside the bitmap should be mapped back into the bitmap by repeating it.
            else if (tileMode == GTileMode::kRepeat) {
                newX = GFloorToInt(temp.x) % fDevice.width();
                newY = GFloorToInt(temp.y) % fDevice.height();
                // If negative (after wrapping), we add fDevice.width() or fDevice.height() to bring it back into the positive range.
                if (newX < 0) newX += fDevice.width();
                if (newY < 0) newY += fDevice.height();

                newX = std::max(0, std::min(fDevice.width() - 1, newX));
                newY = std::max(0, std::min(fDevice.height() - 1, newY));
            }

            // If the coordinate is outside the bitmap, it reflects as if it hit a mirror.
            else if (tileMode == GTileMode::kMirror) {

                float normX = GFloorToInt(temp.x) % (2*fDevice.width());
                float normY = GFloorToInt(temp.y) % (2*fDevice.height());

                // Mirror X coordinate
                if (normX < 0) {
                    normX += 2*fDevice.width();
                }
                if (normX >= fDevice.width()){
                    normX = 2*fDevice.width()-1-normX;
                }
                if (normY < 0) {
                    normY += 2*fDevice.height();
                } 
                if (normY >= fDevice.height()){
                    normY = 2*fDevice.height()-1-normY;
                }
                newX = std::min(std::max(GRoundToInt(normX), 0), fDevice.width() - 1);
                newY = std::min(std::max(GRoundToInt(normY), 0), fDevice.height() - 1);
            }

            row[i] = *fDevice.getAddr(newX, newY);
            temp.x += inverseMatrix[0];
            temp.y += inverseMatrix[1];
        }
    }

    private:

    GMatrix inverseMatrix;
    GMatrix localMatrix;
    GBitmap fDevice;
    GTileMode tileMode;
    float width_inv, height_inv;

};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& matrix, GTileMode mode) {
    if (!bitmap.pixels()) { return nullptr; }
    return std::unique_ptr<GShader>(new BitmapShader(bitmap, matrix, mode));
}


