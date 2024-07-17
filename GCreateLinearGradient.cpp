/*
 *  Copyright 2024 <me>
 */

#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include "include/GPixel.h"
#include <optional>

#include <memory>

class LinearGradient : public GShader {
public:

    LinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode mode) {
        numColors = count;

        for (int i = 0; i<numColors; i++){
            colorsVec.push_back(colors[i]);
        }

        float u = p1.x - p0.x;
        float v = p1.y - p0.y;

        localMatrix = GMatrix(u, -v, p0.x, v, u, p0.y);
        tileMode = mode;
    }

    bool isOpaque() override {
        return false;
    }

    bool setContext (const GMatrix& ctm) override {
        GMatrix temp = GMatrix::Concat(ctm, localMatrix);

        std::optional<GMatrix> invertedCtm = temp.invert();

        if (!(invertedCtm).has_value()){
            return false;
        }
        
        inverseMatrix = *invertedCtm;
        return true;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPoint local = inverseMatrix *  GPoint{x + 0.5f, y + 0.5f};

        for (int i=0; i < count; i++){
            GColor c;
            if (numColors == 1) {
                c = colorsVec.front();
            } 
            else {
                float x_ = local.x;
            
        
                if (tileMode == GTileMode::kClamp) {
                    x_ = GPinToUnit(x_);
                }
                else if (tileMode == GTileMode::kRepeat) {
                    x_ -= GFloorToInt(x_);
                }
                else if (tileMode == GTileMode::kMirror) {
                    if ((int) floorf(x_)%2 == 0) {
                        x_ -= floorf(x_);
                    } else {
                        x_ = 1.0f - (x_-floorf(x_));
                    }
                }

                int num_intervals = colorsVec.size() - 1;
                int index = (int) (x_ * num_intervals);
                float u = (x_ - (1.0f / num_intervals) * index) * num_intervals;

                GColor c1 = colorsVec[index + 1];
                GColor c2 = colorsVec[index];

                c.a = c1.a * u + c2.a * (1 - u);
                c.r = c1.r * u + c2.r * (1 - u);
                c.g = c1.g * u + c2.g * (1 - u);
                c.b = c1.b * u + c2.b * (1 - u);
            }

            row[i] = GPixel_PackARGB(GRoundToInt(c.a * 255), GRoundToInt(c.a * c.r * 255), GRoundToInt(c.a * c.g * 255), GRoundToInt(c.a * c.b * 255));
            local.x += inverseMatrix[0];
            local.y += inverseMatrix[3];
        }
    }

private:
    bool opaque;
    GMatrix inverseMatrix;
    GMatrix localMatrix;
    int numColors;
    std::vector<GColor> colorsVec;
    GTileMode tileMode;
};

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode mode) {
     if (count < 1) { return nullptr; }
    return std::unique_ptr<GShader>(new LinearGradient(p0, p1, colors, count, mode));
}
