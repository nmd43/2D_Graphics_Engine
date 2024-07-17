/*
 *  Copyright 2024 <me>
 */

#ifndef _g_starter_canvas_h_
#define _g_starter_canvas_h_

#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GPath.h"
#include <stack>
#include <vector>

// Define Edge struct
struct Edge {
    GPoint p0;
    GPoint p1;
    int winding;
    bool isValid(int y) const;
    float computeX(int y) const;
};

typedef GPixel (*BlendProc)(GPixel*, GPixel);


class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device);
    virtual void clear(const GColor& color) override;
    void fillRect(const GRect& rect, const GColor& color);
    virtual void drawRect(const GRect& rect, const GPaint& paint) override;
    void blend(GPixel* destPixel, int y, int startX, int endX, GPixel sourcePixel, BlendProc proc);
    void shadeOrBlend(GPixel* destRow, int y, int startX, int endX, BlendProc proc, GColor color, GShader* shader, bool useShader);
    void shadeBlend(GPixel* destRow, int y, int startX, int endX, GPixel temp[], BlendProc proc);
    virtual void drawConvexPolygon(const GPoint[], int count, const GPaint& paint) override;
    virtual void drawPath(const GPath &, const GPaint &) override;
    void clipEdge(GPoint& p0, GPoint& p1);
    GPoint makeGPoint(float, float);
    GColor makeNewC(const GColor colors[4], float u, float v);
    GPoint makeNewP(const GPoint points[4], float u, float v);
    virtual void save() override;
    virtual void restore() override;
    virtual void concat(const GMatrix&) override;
    virtual void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) override;
    virtual void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint&) override;

private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    std::stack<GMatrix> matrixStack;
    std::vector<Edge> edges;
};

#endif