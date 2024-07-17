#include "include/GShader.h"
#include "include/GMatrix.h"

class TextureProxyShader : public GShader {
    GShader* fRealShader;
    GMatrix  fExtraTransform;
public:
    TextureProxyShader(GShader* shader, const GMatrix& extraTransform)
        : fRealShader(shader), fExtraTransform(extraTransform) {}

    bool isOpaque() override { return fRealShader->isOpaque(); }

    bool setContext(const GMatrix& ctm) override {
        return fRealShader->setContext(GMatrix::Concat(ctm, fExtraTransform));
    }
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        fRealShader->shadeRow(x, y, count, row);
    }
};
