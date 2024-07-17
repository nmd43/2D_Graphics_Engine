/**
 *  Copyright 2019 Mike Reed
 */

#include <vector>

class TrivialShader : public GShader {
public:
    TrivialShader() {}
    bool isOpaque() override { return true; }
    bool setContext(const GMatrix&) override { return true; }
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; ++i) {
            row[i] = 0xFF808080;
        }
    }
};

class MeshBench : public GBenchmark {
    std::vector<GPoint> fVerts;
    std::vector<GColor> fColors;
    std::vector<GPoint> fTexs;
    std::vector<int>    fIndices;
    const char*         fName;

public:
    MeshBench(const GPoint v[], const GColor c[], const GPoint t[], int N, const int x[],
              const char name[]) : fName(name)
    {
        int max_index = 0;
        for (int i = 0; i < N*3; ++i) {
            max_index = std::max(max_index, x[i]);
            fIndices.push_back(x[i]);
        }
        for (int i = 0; i <= max_index; ++i) {
            fVerts.push_back(v[i]);
            if (c) {
                fColors.push_back(c[i]);
            }
            if (t) {
                fTexs.push_back(t[i]);
            }
        }
    }

    const char* name() const override { return fName; }
    GISize size() const override { return { 100, 100 }; }

    void draw(GCanvas* canvas) override {
        TrivialShader shader;
        GPaint paint;
        if (fTexs.size()) {
            paint.setShader(&shader);
        }
        for (int i = 0; i < 50; ++i) {
            canvas->drawMesh(&fVerts[0],
                             fColors.size() ? &fColors[0] : nullptr,
                             fTexs.size() ? &fTexs[0] : nullptr,
                             fIndices.size()/3, &fIndices[0], paint);
        }
    }
};

class QuadBench : public GBenchmark {
    std::vector<GColor> fColors;
    std::vector<GPoint> fTexs;
    const char*         fName;

public:
    QuadBench(const GColor c[], const GPoint t[], const char name[]) : fName(name) {
        if (c) {
            fColors.insert(fColors.begin(), c, c + 4);
        }
        if (t) {
            fTexs.insert(fTexs.begin(), t, t + 4);
        }
    }

    const char* name() const override { return fName; }
    GISize size() const override { return { 100, 100 }; }

    void draw(GCanvas* canvas) override {
        const GPoint verts[] = {
            {0, 1}, {10, 0}, {10, 10}, {0, 9},
        };
        TrivialShader shader;
        GPaint paint;
        const int levels = 32;
        if (fTexs.size()) {
            paint.setShader(&shader);
        }
        for (int i = 0; i < 5; ++i) {
            canvas->drawQuad(verts,
                             fColors.size() ? fColors.data() : nullptr,
                             fTexs.size() ? fTexs.data() : nullptr,
                             levels,
                             paint);
        }
    }
};
