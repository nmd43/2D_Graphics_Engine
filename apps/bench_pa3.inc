/**
 *  Copyright 2019 Mike Reed
 */

#include "../include/GShader.h"

class ShaderBench : public GBenchmark {
protected:
    enum { W = 200, H = 200 };
    const char* fName;
    const int fLoops;
    std::unique_ptr<GShader> fShader;

    ShaderBench(const char* name, int loops) : fName(name), fLoops(loops) {}

public:
    const char* name() const override { return fName; }
    GISize size() const override { return { W, H }; }

    void draw(GCanvas* canvas) override {
        const GRect r = {0, 0, W, H};
        GPaint paint(fShader.get());
        for (int i = 0; i < fLoops; ++i) {
            canvas->drawRect(r, paint);
        }
    }
};

class BitmapBench : public ShaderBench {
public:
    BitmapBench(const char imagePath[], const char* name, GTileMode mode = GTileMode::kClamp)
        : ShaderBench(name, 50)
    {
        GBitmap bm;
        bm.readFromFile(imagePath);
        GMatrix mx = GMatrix::Scale(1.0f * W / bm.width(), 1.0f * H / bm.height());
        fShader = GCreateBitmapShader(bm, mx, mode);
    }
};

