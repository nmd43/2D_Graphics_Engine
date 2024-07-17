/**
 *  Copyright 2018 Mike Reed
 */

#include "../include/GMatrix.h"
#include "../include/GPath.h"
#include "../include/GTime.h"

static void offset_pts(GPoint p[], int count, GVector d) {
    for (int i = 0; i < count; ++i) {
        p[i] = p[i] + d;
    }
}

static float compute_cross(GPoint a, GPoint b, GPoint c) {
    GPoint ac = { c.x - a.x, c.y - a.y };
    GPoint ab = { b.x - a.x, b.y - a.y };
    return ac.x * ab.y - ac.y * ab.x;
}

static bool inside(const GPoint p[], int count, GPoint target) {
    if (count <= 2) {
        return false;
    }
    float cross = compute_cross(p[count-1], p[0], target);
    for (int i = 0; i < count - 1; ++i) {
        float c = compute_cross(p[i], p[i+1], target);
        if ((cross < 0) != (c < 0)) {
            return false;
        }
    }
    return true;
}

static float draw_arrows(GCanvas* canvas, GPoint p0, GPoint p1, const GColor& color,
                         float period, float phase) {
    const GVector v = p1 - p0;
    const float len = v.length();
    const GVector uv = v * (1 / len);
    const GVector un = { uv.y, -uv.x };
    
    float dist = phase;
    while (dist < len) {
        GPoint c = p0 + uv * dist;
        GPoint pts[3] = { c + un * 4, c - un * 4, c + uv * 12 };
        canvas->drawConvexPolygon(pts, 3, GPaint(color));
        dist += period;
    }
    return dist - len;
}

static void set_rect_pts(GPoint pts[4], const GRect& r) {
    pts[0] = { r.left,  r.top };
    pts[1] = { r.right, r.top };
    pts[2] = { r.right, r.bottom };
    pts[3] = { r.left,  r.bottom };
}

static void make_star(GPoint pts[], int count, float anglePhase) {
    assert(count & 1);
    float da = 2 * M_PI * (count >> 1) / count;
    float angle = anglePhase;
    for (int i = 0; i < count; ++i) {
        pts[i].x = cosf(angle);
        pts[i].y = sinf(angle);
        angle += da;
    }
}

class PolyShape : public Shape {
    struct Cntr {
        int     fCount;
        GPoint* fPts;
    };
    GPoint fPts[10];
    Cntr   fCtrs[2];
    int fPtCount;
    int fCtrCount;
public:
    PolyShape(GColor c, int ctrCount) : fColor(c) {
        if (ctrCount == 2) {
            fPtCount = 8;
            fCtrCount = 2;
            set_rect_pts(fPts, GRect::XYWH(100, 100, 200, 150));
            set_rect_pts(fPts + 4, GRect::XYWH(150, 150, 100, 50));
            fCtrs[0] = { 4, &fPts[0] };
            fCtrs[1] = { 4, &fPts[4] };
        } else if (ctrCount == 1) {
            fPtCount = 7;
            fCtrCount = 1;
            make_star(fPts, 7, 0);
            (GMatrix::Translate(200, 200) * GMatrix::Scale(100, 100)).mapPoints(fPts, 7);
            fCtrs[0] = { 7, &fPts[0] };
        }
    }

    GPath& makePath(GPath* path) const {
        for (int i = 0; i < fCtrCount; ++i) {
            path->addPolygon(fCtrs[i].fPts, fCtrs[i].fCount);
        }
        return *path;
    }

    void onDraw(GCanvas* canvas, const GPaint& paint) override {
        GPath path;
        canvas->drawPath(this->makePath(&path), paint);
    }
    void drawHilite(GCanvas* canvas) override {
        GPaint p;
        for (int i = 0; i < fPtCount; ++i) {
            canvas->drawRect(GRect::XYWH(fPts[i].x - 2, fPts[i].y - 2, 5, 5), p);
        }
        
        const double speed = 32;
        const float period = 48.5;
        float phase = fmod(GTime::GetMSec() * speed / 1000.0, period);

        int base = 0;
        for (int j = 0; j < fCtrCount; ++j) {
            for (int i = 0; i < fCtrs[j].fCount; ++i) {
                GPoint p0 = fPts[base + i],
                p1 = fPts[base + ((i + 1) % fCtrs[j].fCount)];
                draw_line(canvas, p0, p1, {0,0,0,1}, 1);

                phase = draw_arrows(canvas, p0, p1, {0,0,0,1}, period, phase);
            }
            base += fCtrs[j].fCount;
        }

        this->Shape::drawShaderHilite(canvas);
    }

    GRect getRect() override {
        GPath path;
        return this->makePath(&path).bounds();
    }

    void setRect(const GRect& r) override {}
    GColor onGetColor() override { return fColor; }
    void onSetColor(const GColor& c) override { fColor = c; }
    bool animates() const override { return true; }

    GClick* findClick(GPoint p, GWindow* wind) override {
        if (GClick* click = Shape::findClick(p, wind)) {
            return click;
        }

        int index = -1;
        for (int i = 0; i < fPtCount; ++i) {
            if (hit_test(p.x, p.y, fPts[i].x, fPts[i].y)) {
                index = i;
            }
        }
        if (index >= 0) {
            return new GClick(p, [this, wind, index](GClick* click) {
                fPts[index] = click->curr();
                wind->requestDraw();
            });
        }
        index = -1;
        for (int i = 0; i < fCtrCount; ++i) {
            if (inside(fCtrs[i].fPts, fCtrs[i].fCount, p)) {
                index = i;
                break;
            }
        }
        if (index >= 0) {
            return new GClick(p, [this, wind, index](GClick* click) {
                offset_pts(const_cast<GPoint*>(fCtrs[index].fPts), fCtrs[index].fCount,
                       click->curr() - click->prev());
                wind->requestDraw();
            });
        }

        return nullptr;
    }
private:
    GColor  fColor;
};

