/**
 *  Copyright 2023 Mike Reed
 */

#include "../include/GPoint.h"
#include "../include/GMatrix.h"
#include "../include/GCanvas.h"
#include <vector>

template <typename T> T lerp(T a, T b, float t) {
    return a + (b - a)*t;
}

static bool nearly_equal(GPoint a, GPoint b, float tol) {
    auto vec = b - a;
    return vec.length() <= tol;
}

static void compute_pos_tan(const GPoint pts[4], float t, GPoint* pos, GPoint* tan) {
    GPoint A = (pts[3] - pts[0]) + 3.0f*(pts[1] - pts[2]);
    GPoint B = 3.0f*((pts[2] - pts[1]) + (pts[0] - pts[1]));
    GPoint C = 3.0f*(pts[1] - pts[0]);
    GPoint D = pts[0];

    if (pos) {
        *pos = ((A*t + B)*t + C)* t + D;
    }

    if (tan) {
        // F' = 3At^2 + 2Bt + C
        *tan = (3*A*t + 2*B)*t + C;
        *tan = *tan * (1.0f / tan->length());
    }
}

static void add_line(GPath* path, GPoint a, GPoint b, float width) {
    const float radius = width * 0.5f;
    auto vec = b - a;
    vec = vec * (radius / vec.length());
    GPoint norm = {vec.y, -vec.x};
    path->moveTo(a + norm);
    path->lineTo(b + norm);
    path->lineTo(b - norm);
    path->lineTo(a - norm);
}

static void frame_line(GCanvas* canvas, GPoint a, GPoint b, const GPaint& paint, float width) {
    GPath path;
    add_line(&path, a, b, width);
    canvas->drawPath(path, paint);
}

static void draw_mesh_frame(GCanvas* canvas, const GPoint verts[], const int indices[], int nTris) {
    GPaint paint;
    const float w = 1.5f;
    auto frame_triangle = [&](GPoint a, GPoint b, GPoint c) {
        GPath path;
        add_line(&path, a, b, w);
        add_line(&path, b, c, w);
        add_line(&path, c, a, w);
        canvas->drawPath(path, paint);
    };
    for (int i = 0; i < nTris; ++i) {
        frame_triangle(verts[indices[i*3 + 0]],
                       verts[indices[i*3 + 1]],
                       verts[indices[i*3 + 2]]);
    }
}

static void draw_mesh_quads(GCanvas* canvas, const GPoint verts[], int nQuads) {
    GPaint paint;
    const float w = 1.5f;
    for (int i = 0; i < nQuads; ++i) {
        frame_line(canvas, verts[i*2 + 0], verts[i*2 + 2], paint, w);
        frame_line(canvas, verts[i*2 + 1], verts[i*2 + 3], paint, w);
        frame_line(canvas, verts[i*2 + 0], verts[i*2 + 1], paint, w);
    }
    frame_line(canvas, verts[nQuads*2 + 0], verts[nQuads*2 + 1], paint, w);
}

class StripShape : public Shape {
    enum { BM_N = 5 };
    GPoint  fPts[4];
    GColor  fColors[4];
    GBitmap fBitmaps[BM_N];

    float fCntrlPointRadius = 4;
    float fRadius = 30;
    int   fSegs = 20;
    GShader::TileMode fTile = GShader::kRepeat;
    int   fBitmapIndex = 0;
    bool  fShowFrame = true;
    bool  fUseBitmap = false;

public:
    StripShape() {
        fPts[0] = { 50, 150};
        fPts[1] = {150,  50};
        fPts[2] = {250, 250};
        fPts[3] = {350, 150};

        fColors[0] = { 1, 0, 0, 1 };
        fColors[1] = { 0, 1, 0, 1 };
        fColors[2] = { 0, 0, 1, 1 };
        fColors[3] = { 1, 1, 0, 1 };

        for (int i = 0; i < BM_N; ++i) {
            std::string str("apps/wood#.png");
            str[9] = '0' + i;
            fBitmaps[i].readFromFile(str.c_str());
        }
    }

    static GColor color_proc(const GColor[], float u, float v) {
        float uu = sin(u * M_PI);
        float vv = sin(v * M_PI);
        float scale = powf(uu * vv, fExp);
        return { scale, scale, scale, 1 };
    }

    static void set_quad_tris(int indices[], int i) {
        indices[0] = i; indices[1] = i+1; indices[2] = i+3;
        indices[3] = i; indices[4] = i+3; indices[5] = i+2;
    }

    static void draw_circle(GCanvas* canvas, GPoint center, float rad, const GPaint& paint) {
        GPath path;
        path.addCircle(center, rad);
        canvas->drawPath(path, paint);
    }

    static void frame_cubic(GCanvas* canvas, const GPoint cubic[4], const GColor c) {
        GPaint paint(c);
        const int N = 32;
        GPoint prev = cubic[0];
        for (int i = 1; i <= N; ++i) {
            GPoint pos;
            compute_pos_tan(cubic, i * 1.0f / N, &pos, nullptr);
            frame_line(canvas, prev, pos, paint, 1.5f);
            prev = pos;
        }
    }

    void onDraw(GCanvas* canvas, const GPaint&) override {
        GPaint paint;

        const int nTris = fSegs * 2;
        const int nPts = (fSegs + 1) * 2;
        std::vector<GPoint> verts(nPts);
        std::vector<GPoint> texrs(nPts);
        std::vector<GColor> colrs(nPts);
        std::vector<int>    index(nTris * 3);

        const GBitmap& bm = fBitmaps[fBitmapIndex];
        const float texw = bm.height() / (fRadius * 2);
        const float texh = bm.height();

        std::unique_ptr<GShader> shader;
        if (fUseBitmap) {
            shader = GCreateBitmapShader(bm, GMatrix(), fTile);
            paint.setShader(shader.get());
        }
        /** Distribute the points above/below, above/below each point on the curve
         *
         *  0    2     4     6
         *  |  /  |  /  |   /   |
         *  1    3     5     7
         */

        float arcLength = 0;
        GPoint prevPos = fPts[0];

        for (int i = 0; i <= fSegs; ++i) {
            const float t = i * 1.0f / fSegs;
            assert(t >= 0 && t <= 1);

            GPoint pos, tan;
            compute_pos_tan(fPts, t, &pos, &tan);
            GPoint normal = GPoint{tan.y, -tan.x} * fRadius;

            arcLength += (pos - prevPos).length();
            prevPos = pos;

            verts[i*2 + 0] = pos + normal;
            verts[i*2 + 1] = pos - normal;

            colrs[i*2 + 0] = lerp(fColors[0], fColors[2], t);
            colrs[i*2 + 1] = lerp(fColors[1], fColors[3], t);

            texrs[i*2 + 0] = GPoint{arcLength * texw, 0};
            texrs[i*2 + 1] = GPoint{arcLength * texw, texh};
        }

        for (int i = 0; i < fSegs; ++i) {
            set_quad_tris(&index[i*6], i * 2);
        }

        if (fShowFrame) {
            draw_mesh_frame(canvas, verts.data(), index.data(), nTris);
            if (false) {
                draw_mesh_quads(canvas, verts.data(), nTris >> 1);
            }
            frame_cubic(canvas, fPts, GColor{1,0,0,1});
        } else {
            const GPoint* txs = nullptr;
            const GColor* clr = nullptr;
            if (fUseBitmap) {
                txs = texrs.data();
            } else {
                clr = colrs.data();
            }
            canvas->drawMesh(verts.data(), clr, txs, nTris, index.data(), paint);
        }

        paint.setColor({0, 0, 0, 1});
        for (auto p : fPts) {
            draw_circle(canvas, p, fCntrlPointRadius, paint);
        }
    }

    GRect getRect() override {
        return GRect::WH(200, 200);
    }

    GColor onGetColor() override { return fColors[0]; }
    void onSetColor(const GColor& c) override { fColors[0] = c; }

    GClick* findClick(GPoint pt, GWindow* wind) override {
        for (int i = 0; i < 4; ++i) {
            if (nearly_equal(fPts[i], pt, fCntrlPointRadius*2)) {
                return new GClick(pt, [this, wind, i](GClick* click) {
                    fPts[i] = click->curr();
                    wind->requestDraw();
                });
            }
        }
        return new GClick(pt, [this, wind](GClick* click) {
            for (auto& p : fPts) {
                p += click->curr() - click->prev();
            }
            wind->requestDraw();
        });
    }

    void drawHilite(GCanvas*) override {  }

    bool handleSym(uint32_t sym) override {
        switch (sym) {
            case '-': fSegs = std::max(fSegs - 1, 2); return true;
            case '=': fSegs += 1; return true;
            case '[': fRadius = std::max(fRadius - 2, 4.0f); return true;
            case ']': fRadius += 2; return true;
            case 'b': fUseBitmap = !fUseBitmap; return true;
            case 'f': fShowFrame = !fShowFrame; return true;
            case 'i': fBitmapIndex = (fBitmapIndex + 1) % BM_N; return true;
            case 't': fTile = (fTile == GShader::kRepeat) ? GShader::kMirror : GShader::kRepeat;
            default:
                break;
        }
        return false;
    }
};

Shape* StripShape_Factory() { return new StripShape; }
