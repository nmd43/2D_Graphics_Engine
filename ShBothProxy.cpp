#include "include/GShader.h"
#include "include/GMatrix.h"

class BothProxyShader : public GShader {
    GShader* fShadeColor;
    GShader*  fShadeTexture;
public:
    BothProxyShader(GShader* colorShader, GShader* textureShader)
        : fShadeColor(colorShader), fShadeTexture(textureShader) {}

    bool isOpaque() override { return fShadeColor->isOpaque() && fShadeTexture->isOpaque(); }

    bool setContext(const GMatrix& ctm) override {
        fShadeColor->setContext(ctm);
        fShadeTexture->setContext(ctm);
        return true;
    }
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel rowColor[count+1];
        GPixel rowTexture[count+1];
        fShadeColor->shadeRow(x, y, count, rowColor);
        fShadeTexture->shadeRow(x, y, count, rowTexture);
        for (int i=0; i<count; i++){
            int a = GRoundToInt(divide(GPixel_GetA(rowColor[i]) * GPixel_GetA(rowTexture[i])));
            int r = GRoundToInt(divide(GPixel_GetR(rowColor[i]) * GPixel_GetR(rowTexture[i])));
            int g = GRoundToInt(divide(GPixel_GetG(rowColor[i]) * GPixel_GetG(rowTexture[i])));
            int b = GRoundToInt(divide(GPixel_GetB(rowColor[i]) * GPixel_GetB(rowTexture[i])));
            row[i] = GPixel_PackARGB(a, r, g, b);
        }
    }

    int divide(int num) {
        return (num + 128) * 257 >> 16;
    }
};
