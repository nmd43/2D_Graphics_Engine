/**
 *  Copyright 2018 Mike Reed
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
#include <string>

static void draw_tri(GCanvas* canvas) {
    const GPoint pts[] = {
        { 10, 10 }, { 400, 100 }, { 250, 400 },
    };
    const GColor clr[] = {
        { 1, 0, 0, 1 }, { 0, 1, 0, 1 }, { 0, 0, 1, 1 },
    };
    const int indices[] = {
        0, 1, 2,
    };

    canvas->translate(250, 250);
    canvas->rotate(gFloatPI/2);
    canvas->translate(-250, -250);
    canvas->drawMesh(pts, clr, nullptr, 1, indices, GPaint());
}

static void draw_tri2(GCanvas* canvas) {
    GBitmap bm;
    bm.readFromFile("apps/spock.png");
    float w = bm.width();
    float h = bm.height();

    const GPoint pts[] = {
        { 10, 10 }, { 400, 100 }, { 250, 400 },
    };
    const GPoint tex[] = {
        { 0, 0 }, { w, 0 }, { 0, h },
    };
    const int indices[] = {
        0, 1, 2,
    };

    auto sh = GCreateBitmapShader(bm, GMatrix());
    GPaint paint(sh.get());

    canvas->translate(250, 250);
    canvas->rotate(gFloatPI/2);
    canvas->translate(-250, -250);
    canvas->drawMesh(pts, nullptr, tex, 1, indices, paint);
}

const float twopi = (float)(2*gFloatPI);

static void circular_mesh(GCanvas* canvas, bool showColors, bool showTex) {
    auto shader = GCreateLinearGradient({0, 0}, {1,1}, {0.5,0.25f,0.5,1}, {1,1,1,1});

    const int TRIS = 31;
    GPoint pts[TRIS + 1];
    GColor colors[TRIS + 1];
    GPoint tex[TRIS + 1];
    int indices[TRIS * 3];

    const float radius = 250;
    const float center = 256;
    pts[0] = { center, center };
    colors[0] = { 0.5f, 0, 0.75f, 1 };
    colors[0] = {1,1,1,1};
    tex[0] = { 0, 0 };

    auto wave = [](float radians) {
        return (std::sin(radians) + 1) * 0.5f;
    };
    float angle = 0;
    float da = twopi / (TRIS - 1);
    int* idx = indices;
    for (int i = 1; i <= TRIS; ++i) {
        float x = cos(angle);
        float y = sin(angle);
        const float rad = radius - float(i&1) * 50;
        pts[i] = { x * rad + center, y * rad + center };
        colors[i] = { wave(angle*2), wave(angle*2.5f), wave(angle*3), 1 };
        tex[i] = { angle / twopi, 1 };
        idx[0] = 0; idx[1] = i; idx[2] = i < TRIS ? i + 1 : 1;
        idx += 3;
        angle += da;
    }

    const GColor* colorPtr = showColors ? colors : nullptr;
    const GPoint* texPtr = showTex ? tex : nullptr;
    canvas->drawMesh(pts, colorPtr, texPtr, TRIS, indices, GPaint(shader.get()));
}

static void mesh_1(GCanvas* canvas) { circular_mesh(canvas, true, false); }
static void mesh_3(GCanvas* canvas) { circular_mesh(canvas, true, true); }

static void draw_quad(GCanvas* canvas, const GColor colors[], const GPoint texs[], GShader* shader) {
    GPoint verts[4] = {
        {50, 120}, {470.140625f, 86.2226562f},
        {137.21875f, 332.925781f}, {458.402344f, 450.890625},
    };
    GPaint paint;
    paint.setShader(shader);
    canvas->drawQuad(verts, colors, texs, 12, paint);
}

static void spock_quad(GCanvas* canvas) {
    GBitmap bitmap;
    bitmap.readFromFile("apps/spock.png");
    const float w = bitmap.width();
    const float h = bitmap.height();
    auto shader = GCreateBitmapShader(bitmap, GMatrix(), GTileMode::kMirror);
    const GPoint texs[4] = {
            {w, 0}, {2*w, 0}, {2*w, h}, {w, h},
    };
    draw_quad(canvas, nullptr, texs, shader.get());
}

static void color_quad(GCanvas* canvas) {
    const GColor colors[] = {
            {1, 0, 0, 1},
            {0, 1, 0, 1},
            {0, 0, 1, 1},
            {1, 1, 0, 1},
    };
    draw_quad(canvas, colors, nullptr, nullptr);
}
