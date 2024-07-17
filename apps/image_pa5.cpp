/**
 *  Copyright 2015 Mike Reed
 */

#include "image.h"
#include "../include/GCanvas.h"
#include "../include/GBitmap.h"
#include "../include/GColor.h"
#include "../include/GMatrix.h"
#include "../include/GPath.h"
#include "../include/GPoint.h"
#include "../include/GRandom.h"
#include "../include/GRect.h"

static void draw_quad_rotate(GCanvas* canvas) {
    const GPoint pts[] {
        { 0, 0.1f }, { -1.65f, 5 }, { 0, 6 }, { 1.65f, 5 },
    };
    
    GPath path;
    path.moveTo(pts[0]);
    path.quadTo(pts[1], pts[2]);
    path.quadTo(pts[3], pts[0]);

    canvas->translate(150, 150);
    canvas->scale(25, 25);
    
    float steps = 12;
    float r = 0;
    float b = 1;
    float step = 1 / (steps - 1);
    
    GPaint paint;
    
    for (float angle = 0; angle < 2*gFloatPI - 0.001f; angle += 2*gFloatPI/steps) {
        paint.setColor({ r, 0, b, 1 });
        canvas->save();
        canvas->rotate(angle);
        auto sh = GCreateLinearGradient({0,0}, {1,1}, {r,0,1,1}, {1,0,b,1}, GTileMode::kRepeat);
        canvas->drawPath(path, GPaint(sh.get()));
        canvas->restore();
        r += step;
        b -= step;
    }
}

template <typename DRAW> void spin(GCanvas* canvas, int N, DRAW draw) {
    for (int i = 0; i < N; ++i) {
        canvas->save();
        canvas->rotate(2 * gFloatPI * i / N);
        draw(canvas);
        canvas->restore();
    }
}

static void draw_cubics(GCanvas* canvas) {
    GPath path;
    path.moveTo(10, 0);
    path.cubicTo({100, 100}, {100, -120}, {200, 0});
    GRandom rand;
    
    auto rand_color = [&rand]() -> GColor {
        auto r = rand.nextF();
        auto g = rand.nextF();
        auto b = rand.nextF();
        return { r, g, b, 1 };
    };

    canvas->translate(256, 256);
    spin(canvas, 29, [&](GCanvas* canvas) {
        canvas->drawPath(path, GPaint(rand_color()));
    });
}

static void draw_rings(GCanvas* canvas) {
    GPath path;

    auto draw_color_path = [&](float x, float y, GColor c) {
        GPaint paint(c);
        canvas->save();
        canvas->translate(x, y);
        canvas->drawPath(path, paint);
        canvas->restore();
    };

    path = GPath();
    path.addCircle({256, 256}, 200, GPath::kCW_Direction);
    path.addCircle({256, 256}, 160, GPath::kCCW_Direction);
    draw_color_path(0, 0, {0, 1, 0, 1});

    auto r = GRect{0, 0, 200, 200};

    path = GPath();
    path.moveTo(r.left,  r.top);
    path.lineTo(r.right, r.top);
    path.lineTo(r.right, r.bottom);
    path.lineTo(r.left,  r.bottom);
    path.addCircle({100, 100}, 90, GPath::kCCW_Direction);
    draw_color_path(0, 0, {1, 0, 0, 1});

    path = GPath();
    path.moveTo(r.left,  r.top);
    path.lineTo(r.left,  r.bottom);
    path.lineTo(r.right, r.bottom);
    path.lineTo(r.right, r.top);
    path.addCircle({100, 100}, 90, GPath::kCW_Direction);
    draw_color_path(512-200, 512-200, {0, 0, 1, 1});
}

static void draw_bm_tiling(GCanvas* canvas) {
    const GMatrix m = GMatrix::Rotate(gFloatPI/6) * GMatrix::Scale(0.4f, 0.4f);
    GBitmap bm;
    bm.readFromFile("apps/spock.png");
    auto sh = GCreateBitmapShader(bm, m, GTileMode::kRepeat);
    canvas->drawRect(GRect::XYWH(0, 0, 512, 250), GPaint(sh.get()));
    sh = GCreateBitmapShader(bm, m, GTileMode::kMirror);
    canvas->drawRect(GRect::XYWH(0, 262, 512, 250), GPaint(sh.get()));
}

static void draw_cartman(GCanvas* canvas) {
    GPath path;
    GPaint paint;
#include "cartman.475"
}

static void draw_quad(GCanvas* canvas, GPoint p, const GPoint pts[3], GColor c) {
    GPath path;
    path.moveTo(p);
    path.lineTo(pts[0]);
    path.quadTo(pts[1], pts[2]);
    canvas->drawPath(path, GPaint(c));
}

static void draw_cubic(GCanvas* canvas, GPoint p, const GPoint pts[4], GColor c) {
    GPath path;
    path.moveTo(p);
    path.lineTo(pts[0]);
    path.cubicTo(pts[1], pts[2], pts[3]);
    canvas->drawPath(path, GPaint(c));
}

static void draw_divided(GCanvas* canvas) {
    GPoint quad[] = { { 0, 100 }, { 150, -90 }, { 300, 200 } };
    GPoint pivot = (quad[0] + quad[2]) * 0.5f;
    GPoint tmp0[7], tmp1[7], tmp2[7];

    GPath::ChopQuadAt(quad, tmp0, 0.25);
    GPath::ChopQuadAt(&tmp0[2], tmp1, 1.0f / 3);
    GPath::ChopQuadAt(&tmp1[2], tmp2, 0.5);
    draw_quad(canvas, pivot, tmp0, {1,0,0,1});
    draw_quad(canvas, pivot, tmp1, {0,0,1,1});
    draw_quad(canvas, pivot, tmp2, {0,1,0,1});
    draw_quad(canvas, pivot, &tmp2[2], {0.5,0.25,0.75,1});


    GPoint cubic[] = { { 0, 200 }, { 50, 650 }, { 0, 350 }, { 500, 250 } };
    pivot = (cubic[0] + cubic[3]) * 0.5f;
    
    GPath::ChopCubicAt(cubic, tmp0, 0.25);
    GPath::ChopCubicAt(&tmp0[3], tmp1, 1.0f / 3);
    GPath::ChopCubicAt(&tmp1[3], tmp2, 0.5);
    draw_cubic(canvas, pivot, tmp0, {0.5,0.25,0.75,1});
    draw_cubic(canvas, pivot, tmp1, {0,1,0,1});
    draw_cubic(canvas, pivot, tmp2, {0,0,1,1});
    draw_cubic(canvas, pivot, &tmp2[3], {1,0,0,1});
}

static void draw_mirror_ramp(GCanvas* canvas) {
    const GColor colors[] = {
        {1, 1, 1, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {1, 0, 1, 1},
        {0, 0, 1, 1},
        {1, 1, 0, 1},
    };
    constexpr size_t N = sizeof(colors) / sizeof(colors[0]);

    constexpr int ROWS = 6;
    float h = 512.f / ROWS;
    float y = 0;
    GRandom rand;
    GPaint paint;
    GColor c[ROWS+2];
    for (int i = 0; i < ROWS; ++i) {
        const int colorCount = i + 2;
        for (int j = 0; j < colorCount; ++j) {
            c[j] = colors[rand.nextU() % N];
        }
        GRect r = {0, y, 512, y+h};
        auto sh = GCreateLinearGradient({0, 0}, {256, 0}, c, colorCount, GTileMode::kMirror);
        paint.setShader(sh.get());
        canvas->drawRect(r, paint);
        y += h;
    }
}
