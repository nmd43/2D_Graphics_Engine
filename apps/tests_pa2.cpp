/**
 *  Copyright 2021 Mike Reed
 */

#include "../include/GCanvas.h"
#include "../include/GBitmap.h"
#include "../include/GColor.h"
#include "../include/GPoint.h"
#include "../include/GRect.h"
#include "tests.h"

static void test_poly_nodraw(GTestStats* stats) {
    struct Rec {
        int count;
        GPoint pts[4];
    };

    const Rec polys[] = {
        { 0, {} },
        // point values not important given the count
        { 1, {{0,0}} },
        { 2, {{0,0}, {1,1}} },
        // no area
        { 3, {{0,0}, {3,3}, {1,1}} },
        // clipped out
        { 3, {{0,0}, {2,-2}, {3,0}} },
    };
    
    const GPaint paint({1,1,1,1});
    for (const auto& p : polys) {
        GBitmap bm;
        bm.alloc(3, 3);
        auto canvas = GCreateCanvas(bm);
        canvas->drawConvexPolygon(p.pts, p.count, paint);
        EXPECT_TRUE(stats, expect_pixels_value(bm, 0));
    }
}
