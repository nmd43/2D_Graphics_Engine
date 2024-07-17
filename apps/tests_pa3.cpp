/**
 *  Copyright 2022 Mike Reed
 */

static bool ie_eq(float a, float b) {
    return fabs(a - b) <= 0.00001f;
}

static bool is_eq(const GMatrix& m, float a, float c, float e,
                                    float b, float d, float f) {
    //    printf("%g %g %g    %g %g %g\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    return ie_eq(m[0], a) && ie_eq(m[2], c) && ie_eq(m[4], e) &&
           ie_eq(m[1], b) && ie_eq(m[3], d) && ie_eq(m[5], f);
}

static void test_matrix(GTestStats* stats) {
    GMatrix m;
    EXPECT_TRUE(stats, is_eq(m, 1, 0, 0,
                                0, 1, 0));
    m = GMatrix::Translate(2.5, -4);
    EXPECT_TRUE(stats, is_eq(m, 1, 0,  2.5f,
                                0, 1, -4  ));
    m = GMatrix::Scale(2.5, -4);
    EXPECT_TRUE(stats, is_eq(m, 2.5f, 0, 0,
                                0,   -4, 0));
    m = GMatrix::Rotate(gFloatPI/2);
    EXPECT_TRUE(stats, is_eq(m, 0, -1, 0,
                                1,  0, 0));
    m = GMatrix::Rotate(gFloatPI);
    EXPECT_TRUE(stats, is_eq(m, -1,  0, 0,
                                 0, -1, 0));
    m = GMatrix::Rotate(gFloatPI/4);
    const float r2 = sqrt(2.0f)/2;
    EXPECT_TRUE(stats, is_eq(m, r2, -r2, 0,
                                r2,  r2, 0));

    GMatrix m2, m3;
    m = GMatrix::Scale(2, 3);
    m2 = GMatrix::Scale(-1, -2);
    m3 = m * m2;
    EXPECT_TRUE(stats, is_eq(m3, -2, 0, 0, 0, -6, 0));
    m2 = GMatrix::Translate(5, 6);
    m3 = m2 * m;
    EXPECT_TRUE(stats, is_eq(m3, 2, 0, 5, 0, 3, 6));
}

static void test_matrix_inv(GTestStats* stats) {
    GMatrix m;

    auto m2 = m.invert();
    EXPECT_TRUE(stats, m2.has_value() && m == *m2);
    m = GMatrix::Scale(0.5f, 4);
    m2 = m.invert();
    EXPECT_TRUE(stats, m2.has_value() && is_eq(*m2, 2, 0, 0, 0, 0.25f, 0));

    m = GMatrix::Translate(3, 4) * GMatrix::Rotate(gFloatPI/3);
    m2 = m.invert();
    EXPECT_TRUE(stats, m2.has_value());
    auto m3 = m * *m2;
    EXPECT_TRUE(stats, is_eq(m3,  1, 0, 0, 0, 1, 0));
}

static void test_matrix_map(GTestStats* stats) {
    const GPoint src[] = { {0, 0}, {1, 1}, {-3, 4}, {0.5f, -0.125} };
    GPoint dst[4], dst2[4];

    GMatrix m = GMatrix::Translate(3, 4) * GMatrix::Scale(2, 2);
    m.mapPoints(dst, src, 4);
    const GPoint expected[] = { {3, 4}, {5, 6}, {-3, 12}, {4, 3.75f} };
    bool equal = true;
    for (int i = 0; i < 4; ++i) {
        equal &= dst[i] == expected[i];
    }
    EXPECT_TRUE(stats, equal);

    m = GMatrix::Translate(3, 4) * GMatrix::Rotate(gFloatPI/3);
    m.mapPoints(dst, src, 4);
    memcpy(dst2, src, sizeof(src));
    m.mapPoints(dst2, dst2, 4);
    EXPECT_TRUE(stats, memcmp(dst, dst2, sizeof(src)) == 0);
}

////////////

#include "../include/GShader.h"

static void test_clamp_shader(GTestStats* stats) {
    const GPixel R = GPixel_PackARGB(0xFF, 0xFF,    0, 0);
    const GPixel G = GPixel_PackARGB(0xFF,    0, 0xFF, 0);
    GPixel pixels[] = {
        R, R, R, R,
        R, G, G, R,
        R, G, G, R,
        R, R, R, R,
    };
    // Make a bitmap that points into the middle of our pixel array
    GBitmap bm(2, 2, 4 * sizeof(GPixel), &pixels[5], true);

    auto sh = GCreateBitmapShader(bm, GMatrix());
    if (!sh) {
        EXPECT_TRUE(stats, false);
        return;
    }
    EXPECT_TRUE(stats, sh->setContext(GMatrix()));

    GPixel row[4];
    for (int y = -1; y < 3; ++y) {
        sh->shadeRow(-1, y, 4, row);
        // since we're clamping, we should always return G
        bool isG = true;
        for (int x = 0; x < 4; ++x) {
            if (row[x] != G) {
                isG = false;
            }
        }
        EXPECT_TRUE(stats, isG);
    }
}
