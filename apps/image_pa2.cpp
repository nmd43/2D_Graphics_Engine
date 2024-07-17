/**
 *  Copyright 2015 Mike Reed
 */

#include "image.h"
#include "../include/GCanvas.h"
#include "../include/GBitmap.h"
#include "../include/GColor.h"
#include "../include/GPoint.h"
#include "../include/GRandom.h"
#include "../include/GRect.h"
#include <string>

static void make_regular_poly(GPoint pts[], int count, float cx, float cy, float radius) {
    float angle = 0;
    const float deltaAngle = gFloatPI * 2 / count;

    for (int i = 0; i < count; ++i) {
        pts[i] = {cx + std::cos(angle) * radius, cy + std::sin(angle) * radius};
        angle += deltaAngle;
    }
}

static void dr_poly(GCanvas* canvas, float dx, float dy) {
    GPoint storage[12];
    for (int count = 12; count >= 3; --count) {
        make_regular_poly(storage, count, 256, 256, count * 10 + 120);
        for (int i = 0; i < count; ++i) {
            storage[i].x += dx;
            storage[i].y += dy;
        }
        GColor c = GColor::RGBA(std::abs(sinf(count*7)),
                                std::abs(sinf(count*11)),
                                std::abs(sinf(count*17)),
                                0.8f);
        canvas->drawConvexPolygon(storage, count, GPaint(c));
    }
}

static void draw_poly(GCanvas* canvas) {
    dr_poly(canvas, 0, 0);
}

static void draw_poly_center(GCanvas* canvas) {
    dr_poly(canvas, -128, -128);
}

////////////////////////////////////////////////////////////////////////////////////

static void outer_frame(GCanvas* canvas, const GRect& r) {
    GPaint paint;
    canvas->drawRect(GRect::XYWH(r.left - 2, r.top - 2, 1, r.height() + 4), paint);
    canvas->drawRect(GRect::XYWH(r.right + 1, r.top - 2, 1, r.height() + 4), paint);
    canvas->drawRect(GRect::XYWH(r.left - 1, r.top - 2, r.width() + 2, 1), paint);
    canvas->drawRect(GRect::XYWH(r.left - 1, r.bottom + 1, r.width() + 2, 1), paint);
}

// so we test the polygon code
static GPoint* rect_pts(const GRect& r, GPoint pts[]) {
    pts[0] = { r.left,  r.top };
    pts[1] = { r.right, r.top };
    pts[2] = { r.right, r.bottom };
    pts[3] = { r.left,  r.bottom };
    return pts;
}

static void draw_mode_sample(GCanvas* canvas, const GRect& bounds, GBlendMode mode) {
    const float dx = bounds.width() / 3;
    const float dy = bounds.height() / 3;

    outer_frame(canvas, bounds);

    GPaint paint;
    GPoint pts[4];

    // dst is red
    paint.setBlendMode(GBlendMode::kSrc);
    GRect r = bounds;
    r.bottom = r.top + dy;
    canvas->drawConvexPolygon(rect_pts(r, pts), 4, paint.setRGBA(0, 0, 0, 0));
    r = r.offset(0, dy);
    canvas->drawConvexPolygon(rect_pts(r, pts), 4, paint.setRGBA(1, 0, 0, 0.5));
    r = r.offset(0, dy);
    canvas->drawConvexPolygon(rect_pts(r, pts), 4, paint.setRGBA(1, 0, 0, 1));

    // src is blue
    paint.setBlendMode(mode);
    r = bounds;
    r.right = r.left + dx;
    canvas->drawRect(r, paint.setRGBA(0, 0, 0, 0));
    r = r.offset(dx, 0);
    canvas->drawRect(r, paint.setRGBA(0, 0, 1, 0.5));
    r = r.offset(dx, 0);
    canvas->drawRect(r, paint.setRGBA(0, 0, 1, 1));
}

static void draw_blendmodes(GCanvas* canvas) {
    canvas->clear({1,1,1,1});

    const float W = 100;
    const float H = 100;
    const float margin = 10;
    float x = margin;
    float y = margin;
    for (int i = 0; i < 12; ++i) {
        GBlendMode mode = static_cast<GBlendMode>(i);
        draw_mode_sample(canvas, GRect::XYWH(x, y, W, H), mode);
        if (i % 4 == 3) {
            y += H + margin;
            x = margin;
        } else {
            x += W + margin;
        }
    }
}
