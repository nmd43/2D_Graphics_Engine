/**
 *  Copyright 2015 Mike Reed
 */

#include "bench.h"
#include "../include/GCanvas.h"
#include "../include/GBitmap.h"
#include "../include/GColor.h"
#include "../include/GRandom.h"
#include "../include/GRect.h"
#include <string>

#include "bench_pa1.inc"
#include "bench_pa2.inc"
#include "bench_pa3.inc"
#include "bench_pa4.inc"
#include "bench_pa5.inc"
#include "bench_pa6.inc"

const GBenchmark::Factory gBenchFactories[] {
    []() -> GBenchmark* { return new ClearBench(); },
    []() -> GBenchmark* { return new RectsBench(false); },
    []() -> GBenchmark* { return new RectsBench(true);  },
    []() -> GBenchmark* {
        return new SingleRectBench({2,2}, GRect::LTRB(-1000, -1000, 1002, 1002), "rect_big");
    },
    []() -> GBenchmark* {
        return new SingleRectBench({1000,1000}, GRect::LTRB(500, 500, 502, 502), "rect_tiny");
    },

    // pa2
    []() -> GBenchmark* { return new PolyRectsBench(false); },
    []() -> GBenchmark* { return new PolyRectsBench(true);  },
    []() -> GBenchmark* { return new CirclesBench(false); },
    []() -> GBenchmark* { return new CirclesBench(true);  },
    []() -> GBenchmark* { return new ModesBench({1, 0.5, 0.25, 0.0}, "modes_0"); },
    []() -> GBenchmark* { return new ModesBench({1, 0.5, 0.25, 0.5}, "modes_x"); },
    []() -> GBenchmark* { return new ModesBench({1, 0.5, 0.25, 1.0}, "modes_1"); },

    // pa3
    []() -> GBenchmark* { return new BitmapBench("apps/spock.png", "bitmap_opaque"); },
    []() -> GBenchmark* { return new BitmapBench("apps/wheel.png", "bitmap_alpha"); },

    // pa4
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 0, 1, 1 }};
        return new GradientBench(colors, 1, "gradient_1");
    },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 0, 0, 1 }, { 0, 1, 1, 1 }};
        return new GradientBench(colors, 2, "gradient_2");
    },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 0, 0, 1 }, { 0, 1, 1, 1 }, {0, 1, 0, 0}};
        return new GradientBench(colors, 3, "gradient_3");
    },
    []() -> GBenchmark* { return new PathBench("path_small", 0.1f, false); },
    []() -> GBenchmark* { return new PathBench("path_big",   1.0f, false); },
    []() -> GBenchmark* { return new PathBench("path_bigc",  1.0f,  true); },

    // pa5
    []() -> GBenchmark* {
        return new PathBench2({256, 256}, 256, "path_unclipped");
    },
    []() -> GBenchmark* {
        return new PathBench2({256, 256}, 1024, "path_clipped");
    },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 0, 0, 1 }, { 0, 1, 1, 1 }};
        return new GradientBench(colors, 2, "gradient_2_repeat", GTileMode::kRepeat);
    },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 0, 0, 1 }, { 0, 1, 1, 1 }};
        return new GradientBench(colors, 2, "gradient_2_mirror", GTileMode::kMirror);
    },
    []() -> GBenchmark* { return new BitmapBench("apps/spock.png", "bitmap_repeat",
                                                 GTileMode::kRepeat); },
    []() -> GBenchmark* { return new BitmapBench("apps/spock.png", "bitmap_mirror",
                                                 GTileMode::kMirror); },

    // pa6
    []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const GColor colors[] = {{ 1,1,0,0 }, { 1,0,1,0 }, {1,0,0,1}, {1,1,1,1}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, colors, nullptr, 2, indices, "mesh_colors");
     },
     []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, nullptr, verts, 2, indices, "mesh_texs");
     },
     []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const GColor colors[] = {{ 1,1,0,0 }, { 1,0,1,0 }, {1,0,0,1}, {1,1,1,1}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, colors, verts, 2, indices, "mesh_both");
     },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1,1,0,0 }, { 1,0,1,0 }, {1,0,0,1}, {1,1,1,1}};
        const GPoint texs[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        return new QuadBench(colors, texs, "quad_mesh");
    },

    nullptr,
};
