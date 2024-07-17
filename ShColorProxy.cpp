#include "include/GShader.h"
#include "include/GMatrix.h"

class ColorProxyShader : public GShader {
    GShader* realShader;
    GMatrix  fExtraTransform;
public:
    ColorProxyShader(GPoint points[], GColor colors[]) {
        fc0 = colors[0];
        fc1 = colors[1];
        fc2 = colors[2];
        fp0 = points[0];
        fp1 = points[1];
        fp2 = points[2];

        GPoint u = fp1 - fp0;
        GPoint v = fp2 - fp0;
        fLocalMatrix = GMatrix(u.x, v.x, fp0.x, u.y, v.y, fp0.y);
        dc0 = GColor::RGBA(fc1.r - fc0.r, fc1.g - fc0.g, fc1.b - fc0.b, fc1.a - fc0.a);
        dc1 = GColor::RGBA(fc2.r - fc0.r, fc2.g - fc0.g, fc2.b - fc0.b, fc2.a - fc0.a);

    }

    bool isOpaque() override { return false; }

    bool setContext(const GMatrix& ctm) override {
        
        GMatrix temp = GMatrix::Concat(ctm, fLocalMatrix);
        std::optional<GMatrix> invertedCtm = temp.invert();

        if (!(invertedCtm).has_value()){
            return false;
        }
        
        fInverse = *invertedCtm;
        return true;
    }
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
    
        // Map the points using fInverse
        GPoint local = fInverse *  GPoint{x + 0.5f, y + 0.5f};

        // Compute initial color using P'
        GColor c;
        c.a = local.x * dc0.a + local.y * dc1.a + fc0.a;
        c.r = local.x * dc0.r + local.y * dc1.r + fc0.r;
        c.g = local.x * dc0.g + local.y * dc1.g + fc0.g;
        c.b = local.x * dc0.b + local.y * dc1.b + fc0.b;
        c = c.pinToUnit();

        // Precompute DC
        GColor dc;
        dc.a = dc0.a * fInverse[0] + dc1.a * fInverse[1];
        dc.r = dc0.r * fInverse[0] + dc1.r * fInverse[1];
        dc.g = dc0.g * fInverse[0] + dc1.g * fInverse[1];
        dc.b = dc0.b * fInverse[0] + dc1.b * fInverse[1];

        // Convert initial color to pixel format and set it to the first pixel
        row[0] = GPixel_PackARGB(GRoundToInt(c.a * 255), GRoundToInt(c.r * c.a * 255), GRoundToInt(c.g * c.a * 255), GRoundToInt(c.b * c.a * 255));

        // Compute colors for the rest of the pixels
        for (int i = 1; i < count; ++i) {
            // Compute next color
            GColor nc;
            nc.a = c.a + dc.a;
            nc.r = c.r + dc.r;
            nc.g = c.g + dc.g;
            nc.b = c.b + dc.b;
            nc = nc.pinToUnit();

            // Convert color to pixel format and set it to the next pixel
            row[i] = GPixel_PackARGB(GRoundToInt(nc.a * 255), GRoundToInt(nc.r * nc.a * 255), GRoundToInt(nc.g * nc.a * 255), GRoundToInt(nc.b * nc.a * 255));

            // Update current color
            c = nc;
        }
    }

private: 
    GMatrix fLocalMatrix;
    GMatrix fInverse;

    GColor dc0;
    GColor dc1;
    GColor fc0;
    GColor fc1;
    GColor fc2;

    GPoint fp0;
    GPoint fp1;
    GPoint fp2;
};
