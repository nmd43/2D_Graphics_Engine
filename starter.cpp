/*
*  Copyright 2024 <me>
*/

#include "starter.h"
#include "ShTextureProxy.cpp"
#include "ShColorProxy.cpp"
#include "ShBothProxy.cpp"
#include <iostream>


MyCanvas::MyCanvas(const GBitmap& device) : fDevice(device) {
    // Push the identity matrix onto the stack
    matrixStack.push(GMatrix());
}

void MyCanvas::clear(const GColor& color) {
    GPixel pixel = GPixel_PackARGB(GRoundToInt(color.a * 255), GRoundToInt(color.a * color.r * 255), GRoundToInt(color.a * color.g * 255), GRoundToInt(color.a * color.b * 255));

    int height = fDevice.height();
    int width = fDevice.width();

    int pixelCount = height * width;

    // Fill the entire device with the given pixel color
    std::fill_n(fDevice.pixels(), pixelCount, pixel);

}

int divide(int num) {
    return (num + 128) * 257 >> 16;
}

GPixel clear_mode(GPixel* destPixel, GPixel sourcePixel) {
    return 0;
}

GPixel src_mode(GPixel* destPixel, GPixel sourcePixel) {
    if (GPixel_GetA(sourcePixel)==0){
        return 0;
    }
    return sourcePixel;
}

GPixel dst_mode(GPixel* destPixel, GPixel sourcePixel) {
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return 0;
    }
    return *destPixel;
}

GPixel src_over(GPixel* destPixel, GPixel sourcePixel){

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return dst_mode(destPixel, sourcePixel);
    }
    else if (sourceA == 255){
        return src_mode(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return src_mode(destPixel, sourcePixel);
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);

    
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);
    
    int resultA = sourceA + divide(destA * (255 - sourceA));
    int resultR = sourceR + divide(destR * (255 - sourceA));
    int resultG = sourceG + divide(destG * (255 - sourceA));
    int resultB = sourceB + divide(destB * (255 - sourceA));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
    
}

GPixel dst_over(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return dst_mode(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return src_mode(destPixel, sourcePixel);
    }
    else if (destA == 255){
        return dst_mode(destPixel, sourcePixel);
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);
    
    int resultA = destA + divide(sourceA * (255 - destA));
    int resultR = destR + divide(sourceR * (255 - destA));
    int resultG = destG + divide(sourceG * (255 - destA));
    int resultB = destB + divide(sourceB * (255 - destA));

    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel src_in(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return 0;
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return 0;
    }
    else if (destA == 255){
        return src_mode(destPixel, sourcePixel);
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);

    
    
    int resultA = divide(destA * sourceA);
    int resultR = divide(destA * sourceR);
    int resultG = divide(destA * sourceG);
    int resultB = divide(destA * sourceB);
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel dst_in(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return 0;
    }
    else if (sourceA == 255){
        return dst_mode(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return 0;
    }
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);

    int resultA = divide(sourceA * destA);
    int resultR = divide(sourceA * destR);
    int resultG = divide(sourceA * destG);
    int resultB = divide(sourceA * destB);
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel src_out(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return 0;
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return src_mode(destPixel, sourcePixel);
    }
    else if (destA == 255){
        return 0;
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);

    
    
    int resultA =  divide(sourceA * (255 - destA));
    int resultR =  divide(sourceR * (255 - destA));
    int resultG =  divide(sourceG * (255 - destA));
    int resultB =  divide(sourceB * (255 - destA));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel dst_out(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return dst_mode(destPixel, sourcePixel);
    }
    else if (sourceA == 255){
        return 0;
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return 0;
    }
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);

    int resultA = divide(destA * (255 - sourceA));
    int resultR = divide(destR * (255 - sourceA));
    int resultG = divide(destG * (255 - sourceA));
    int resultB = divide(destB * (255 - sourceA));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel src_atop(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return dst_mode(destPixel, sourcePixel);
    }
    else if (sourceA == 255){
        return src_in(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return 0;
    }
    else if (destA == 255){
        src_over(destPixel, sourcePixel);
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);
    
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);

    int resultA = divide(sourceA*destA) + divide(destA * (255 - sourceA));
    int resultR = divide(sourceR*destA) + divide(destR * (255 - sourceA));
    int resultG = divide(sourceG*destA) + divide(destG * (255 - sourceA));
    int resultB = divide(sourceB*destA) + divide(destB * (255 - sourceA));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel dst_atop(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return 0;
    }
    else if (sourceA == 255){
        return dst_over(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA ==0){
        return src_mode(destPixel, sourcePixel);
    }
    else if (destA == 255){
        return dst_in(destPixel, sourcePixel);
    }
    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);

    
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);
    
    int resultA = divide(destA*sourceA) + divide(sourceA * (255 - destA));
    int resultR = divide(destR*sourceA) + divide(sourceR * (255 - destA));
    int resultG = divide(destG*sourceA) + divide(sourceG * (255 - destA));
    int resultB = divide(destB*sourceA) + divide(sourceB * (255 - destA));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

GPixel xor_mode(GPixel* destPixel, GPixel sourcePixel) {

    int sourceA = GPixel_GetA(sourcePixel);
    if (sourceA == 0){
        return dst_mode(destPixel, sourcePixel);
    }
    else if (sourceA == 255){
        return src_out(destPixel, sourcePixel);
    }
    int destA = GPixel_GetA(*destPixel);
    if (destA == 0){
        return src_mode(destPixel, sourcePixel);
    }
    else if (destA == 255){
        return dst_out(destPixel, sourcePixel);
    }

    int sourceR = GPixel_GetR(sourcePixel);
    int sourceG = GPixel_GetG(sourcePixel);
    int sourceB = GPixel_GetB(sourcePixel);

    
    int destR = GPixel_GetR(*destPixel);
    int destG = GPixel_GetG(*destPixel);
    int destB = GPixel_GetB(*destPixel);

    int resultA = divide((destA * (255 - sourceA)) + (sourceA * (255 - destA)));
    int resultR = divide((destR * (255 - sourceA)) + (sourceR * (255 - destA)));
    int resultG = divide((destG * (255 - sourceA)) + (sourceG * (255 - destA)));
    int resultB = divide((destB * (255 - sourceA)) + (sourceB * (255 - destA)));
    return GPixel_PackARGB(resultA, resultR, resultG, resultB);
}

typedef GPixel (*BlendProc)(GPixel*, GPixel);

const BlendProc gProcs[] = {
    clear_mode, src_mode, dst_mode, src_over, dst_over, src_in, dst_in, src_out, dst_out, src_atop, dst_atop, xor_mode
};

void MyCanvas::shadeOrBlend(GPixel* destRow, int y, int startX, int endX, BlendProc proc, GColor color, GShader* shader, bool useShader){
    int count = endX-startX;

    if (! useShader) {
        GPixel sourcePixel = GPixel_PackARGB(GRoundToInt(color.a * 255), GRoundToInt(color.a * color.r * 255), GRoundToInt(color.a * color.g * 255), GRoundToInt(color.a * color.b * 255));
        blend(destRow, y, startX, endX, sourcePixel, proc);
    }
    else{

        if (!shader->setContext(matrixStack.top())){
            return;
        }
        GPixel temp[count];
        shader->shadeRow(startX, y, count, temp);
        shadeBlend(destRow, y, startX, endX, temp, proc);
    }
}

void MyCanvas::shadeBlend(GPixel* destRow, int y, int startX, int endX, GPixel temp[], BlendProc proc){
    int count = endX - startX;
    int new_count = count/4;

    for (int i=0; i<new_count; i++){
        destRow[i*4] = proc(&destRow[i*4], temp[i*4]);
        destRow[i * 4 + 1] = proc(&destRow[i * 4 + 1], temp[i*4+1]);
        destRow[i * 4 + 2] = proc(&destRow[i * 4 + 2], temp[i*4+2]);
        destRow[i * 4 + 3] = proc(&destRow[i * 4 + 3], temp[i*4+3]);
    }
    for (int i = new_count * 4; i < count; i++) {
        destRow[i] = proc(&destRow[i], temp[i]);
    }
}

void MyCanvas::blend(GPixel* destRow, int y, int startX, int endX, GPixel sourcePixel, BlendProc proc) {
    int count = endX-startX;
    int new_count = count / 4;

    for (int i = 0; i < new_count; ++i) {
        destRow[i * 4] = proc(&destRow[i * 4], sourcePixel);
        destRow[i * 4 + 1] = proc(&destRow[i * 4 + 1], sourcePixel);
        destRow[i * 4 + 2] = proc(&destRow[i * 4 + 2], sourcePixel);
        destRow[i * 4 + 3] = proc(&destRow[i * 4 + 3], sourcePixel);
    }

    // Process any remaining pixels
    for (int i = new_count * 4; i < count; i++) {
        destRow[i] = proc(&destRow[i], sourcePixel);
    }
}

void MyCanvas::drawRect(const GRect &rect, const GPaint &paint){

    int height = fDevice.height();
    int width = fDevice.width();
    GIRect roundedRect = rect.round();
    int minX = roundedRect.left;
    int maxX = std::min(roundedRect.right, width);
    int minY = roundedRect.top;
    int maxY = std::min(roundedRect.bottom, height);

    GPoint rectPoints[4] = {
        makeGPoint(minX, maxY),
        makeGPoint(maxX, maxY),
        makeGPoint(maxX, minY),
        makeGPoint(minX, minY)
    };

    drawConvexPolygon(rectPoints, 4, paint);
}

// Custom comparator function to sort edges by y-values and then by x-values
bool compareEdges(const Edge& a, const Edge& b) {
    if (a.p1.y == b.p1.y) {
        // If y-values of p1 are equal, sort by x-values 
        float a_x = std::min(a.p0.x, a.p1.x);
        float b_x = std::min(b.p0.x, b.p1.x);
        return a_x < b_x;
    } else {
        // Otherwise, sort by y-values of p1
        return a.p1.y < b.p1.y;
    }
}
    

void MyCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint &paint){
    GPoint transPoints[count];
    matrixStack.top().mapPoints(transPoints, points, count);

    edges.clear();

    for (int i = 0; i < count; ++i) {
        GPoint p0 = transPoints[i];
        GPoint p1 = transPoints[(i + 1) % count];
        clipEdge(p0, p1);
    }

    if (edges.size() == 0) { return; }

    std::sort(edges.begin(), edges.end(), compareEdges);

    int height = fDevice.height();
    int width = fDevice.width();

    GBlendMode mode = paint.getBlendMode();
    GShader* shader = paint.getShader();
    GColor color = paint.getColor();

    BlendProc proc;
    bool useShader = shader != nullptr;

    // Determine blending procedure before shade or blend operation
    if (useShader && shader->isOpaque()) {
    // If shader is opaque, we can potentially optimize by using a simpler blending mode,
    // assuming the shader fully covers the destination pixels.
        proc = gProcs[1]; // Equivalent to src_mode
    } else {
    // Use the paint's blend mode as determined by the paint settings
        proc = gProcs[(int) mode];
    }

    for (int y = 0; y < height; ++y) {
        std::vector<int> intersectingXValues;
        
        // Iterate over each edge to find intersections with the current row
        for (const auto& edge : edges) {
            
            const auto& p0 = edge.p0;
            const auto& p1 = edge.p1;
            
            if(p0.y<y) continue;
            
            // Skip horizontal edges
            if (p0.y == p1.y) continue;

            // Check if the current row intersects with the edge
            if (p1.y <= y && p0.y > y) {
                int xIntersect = GRoundToInt(p1.x + ((y+0.5f) - p1.y) * (p0.x - p1.x) / (p0.y - p1.y));
                intersectingXValues.push_back(xIntersect);
            }

            if (intersectingXValues.size() == 2) break;
        }

        // Sort intersecting x-values from left to right
        std::sort(intersectingXValues.begin(), intersectingXValues.end());

        // Fill the pixels between the intersecting x-values for the current row
        for (size_t i = 0; i + 1 < intersectingXValues.size(); i += 2) {
            int startX = std::max(0, intersectingXValues[i]);
            int endX = std::min(width, intersectingXValues[i + 1]);
            if (startX < endX){
                GPixel* destRow = fDevice.getAddr(startX, y);
                shadeOrBlend(destRow, y, startX, endX, proc, color, shader, useShader);
            }
        }
    }
}

void MyCanvas::clipEdge(GPoint& top, GPoint& bottom){
    GPoint left_newPoint = makeGPoint(-1, -1);
    GPoint right_newPoint = makeGPoint(-1, -1);
    GPoint p0, p1;
    float height = (float) fDevice.height();
    float width = (float) fDevice.width();
    int wind = 1;

    if (top.y > bottom.y) {
        p0 = top;
        p1 = bottom;
        wind = -wind;
    } else {
        p0 = bottom;
        p1 = top;
    }

    float m = (p1.y - p0.y)/(p1.x - p0.x);
    float b = p1.y - (m*p1.x);

    if (p0.y <0 || p1.y > height) {
        return;
    }
    
    if (p1.y<0){
        p1.y = 0.0f;
        if(p1.x != p0.x){
            p1.x = ((p1.y) - b)/m;
        }
    }
    if(p0.y > height){
        p0.y = height;
        if (p1.x != p0.x){
            p0.x = ((p0.y) - b)/m;
        }
    }

    if(p1.x>p0.x){
        if(p1.x < 0){
            p1.x = 0.0f;
            p0.x = 0.0f;
        }
        else if(p0.x <0){
            float newX = 0.0f;
            float newY = b;
            left_newPoint = makeGPoint(newX, newY);
            p0.x = 0.0f;
        }
        if (p0.x > width){
            p1.x = width;
            p0.x = width;
        }
        else if (p1.x > width){
            float newX = width;
            float newY = (m*(newX))+b;
            right_newPoint = makeGPoint(newX, newY);
            p1.x = width;
        }
    }
    else if(p1.x < p0.x){
        if(p0.x < 0){
            p0.x = 0.0f;
            p1.x = 0.0f;
        }
        else if(p1.x <0){
            float newX = 0.0f;
            float newY = b;
            left_newPoint = makeGPoint(newX, newY);
            p1.x = 0.0f;
        }
        if (p1.x > width){
            p0.x = width;
            p1.x = width;
        }
        else if (p0.x > width){
            float newX = width;
            float newY = (m*newX)+b;
            right_newPoint = makeGPoint(newX, newY);
            p0.x = width;
        }
    }

    if (GRoundToInt(p0.y) == GRoundToInt(p1.y)) {
        return;
    }

    if (left_newPoint.x == -1 && right_newPoint.x == -1){
        Edge edge{ p0, p1, wind };
        edges.push_back(edge);

    }
    else if(left_newPoint.x != -1 && right_newPoint.x != -1){
        if(p1.x>p0.x){
            Edge edge1{ p0, left_newPoint, wind };
            Edge edge2{ left_newPoint, right_newPoint, wind };
            Edge edge3{ right_newPoint, p1, wind };
            edges.push_back(edge1);
            edges.push_back(edge2);
            edges.push_back(edge3);
        }
        else if (p1.x<p0.x){
            Edge edge1{ p1, left_newPoint, wind };
            Edge edge2{ left_newPoint, right_newPoint, wind };
            Edge edge3{ right_newPoint, p0, wind };
            edges.push_back(edge1);
            edges.push_back(edge2);
            edges.push_back(edge3);
        }
    }
    else if (left_newPoint.x == -1){
        Edge edge1{ p0, right_newPoint, wind };
        Edge edge2{ right_newPoint, p1, wind };
        edges.push_back(edge1);
        edges.push_back(edge2);
    }
    else if(right_newPoint.x == -1){
       Edge edge1{ p0, left_newPoint, wind };
        Edge edge2{ left_newPoint, p1, wind };
        edges.push_back(edge1);
        edges.push_back(edge2);
    }
}

GPoint MyCanvas::makeGPoint(float x, float y){
    GPoint point;
    point.x = x;
    point.y = y;
    return point;
}

void MyCanvas::save() {
    GMatrix topMatrix = matrixStack.top();
    matrixStack.push(topMatrix);
}

void MyCanvas::restore() {
    matrixStack.pop();
}

void MyCanvas::concat(const GMatrix& matrix) {
    GMatrix topMatrix = matrixStack.top();
    matrixStack.pop();
    topMatrix = GMatrix::Concat(topMatrix, matrix);
    matrixStack.push(topMatrix);
}

bool Edge::isValid(int y) const {
    if (GRoundToInt(p1.y) <= y && GRoundToInt(p0.y) > y){
        return true;
    }
    return false;
}

float Edge::computeX(int y) const {
    return (p1.x + ((y+0.5f) - p1.y) * (p0.x - p1.x) / (p0.y - p1.y));
}


void MyCanvas::drawPath(const GPath& path, const GPaint& paint){

    if (path.countPoints() < 2){
        return;
    }

    GPath trans_path = path;
    trans_path.transform(matrixStack.top());
    GPath::Edger edger = GPath::Edger(trans_path);
    std::optional<GPath::Verb> verb;
    edges.clear();

    do {
        GPoint pts[GPath::kMaxNextPoints];        
        verb = edger.next(pts);
        if (verb == GPath::kLine) {
             clipEdge(pts[0], pts[1]);
        }
        else if (verb == GPath::kQuad) {
            GPoint A = pts[0], B = pts[1], C = pts[2];
            GPoint e = 0.25f * (A - 2*B + C);
            float length_e = sqrt(e.x*e.x + e.y*e.y);
            int numSegs = GCeilToInt(sqrt(length_e/0.25f));


            GPoint p0, p1;
            float inv_seg_num = 1.0f / numSegs;
            for (int i = 0; i <= numSegs; i++) { 
                float t = i * inv_seg_num;
                p1 = ((t * t) * (A - 2 * B + C) + t * (-2 * A + 2 * B) + A);
                if (i != 0) { // Clip only if it's not the first point
                    clipEdge(p0, p1);
                }
                p0 = p1;
            }
        }
        else if (verb == GPath::kCubic) {
            GPoint A = pts[0], B = pts[1], C = pts[2], D = pts[3];
            float e0_x = A.x - 2*B.x + C.x; float e1_x = B.x - 2*C.x + D.x;
            float e0_y = A.y - 2*B.y + C.y; float e1_y = B.y - 2*C.y + D.y;
            float length_e0 = sqrt(e0_x*e0_x + e0_y*e0_y);
            float length_e1 = sqrt(e1_x*e1_x + e1_y*e1_y);
            int numSegs = GCeilToInt(sqrt(0.75f * std::max(length_e0, length_e1)/0.25f));
            GPoint p0, p1;
            float inv_seg_num = 1.0f / numSegs;
            for (int i = 0; i <= numSegs; i++) {
                float t = i * inv_seg_num;
                p1 = ((t*t*t)*(-1*A + 3*B - 3*C + D) + (t*t)*(3*A - 6*B + 3*C) + t*(-3*A + 3*B) + A);
                if (i != 0) { // Clip only if it's not the first point
                    clipEdge(p0, p1);
                }
                p0 = p1;
            }
        }
        
    } while (verb.has_value());


    GBlendMode mode = paint.getBlendMode();
    GShader* shader = paint.getShader();
    GColor color = paint.getColor();

    BlendProc proc;
    bool useShader = shader != nullptr;
    if (useShader && shader->isOpaque()) {
        proc = gProcs[1]; // Equivalent to src_mode
    } else {
        proc = gProcs[(int) mode];
    }

    if (edges.size() < 2) {return; }

    // std::cout << "EDGE SIZE: " << edges.size() << std::endl;

    std::sort(edges.begin(), edges.end(), compareEdges);
    // Erase edges where p1.y == p0.y
    edges.erase(std::remove_if(edges.begin(), edges.end(), [](const Edge& edge) {
        return GRoundToInt(edge.p1.y) == GRoundToInt(edge.p0.y);
    }), edges.end());


    for (int y = 0; y < fDevice.height(); y++){
        size_t i = 0;
        int w = 0;
        int L;
        while (i < edges.size() && edges[i].isValid(y)){

            int x = GRoundToInt(edges[i].computeX(y));
            x  = std::min(std::max(x, 0), fDevice.width() - 1);

            if (w == 0){
                L = x;
            }
            w += edges[i].winding;  // +1 or -1
            if (w == 0) {
                int R = x;
                if (R < L){
                    int temp = L;
                    L = R;
                    R = temp;
                }
                GPixel* destRow = fDevice.getAddr(L, y);
                shadeOrBlend(destRow, y, L, R, proc, color, shader, useShader);
            }

            if (edges[i].isValid(y+1)) {
                i += 1;
            }
            else {
                edges.erase(edges.begin() + i);	// we’re done with this edge
            }
        }

        while (i < edges.size() && edges[i].isValid(y+1)) {
            i += 1;
        }

        std::sort(edges.begin(), edges.begin() + i, [&](const Edge& a, const Edge& b) {
            float xA = a.computeX(y + 1);
            float xB = b.computeX(y + 1);
            return xA < xB;
        });

    }
}

void MyCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint){
    int n = 0;
    for (int i = 0; i < count; i++) {
        GPoint point0 = verts[indices[n+0]];
        GPoint point1 = verts[indices[n+1]];
        GPoint point2 = verts[indices[n+2]];
        GPoint points[3] = {point0, point1, point2};

        if (colors!=NULL && texs!=NULL) {
            GColor color0 = colors[indices[n+0]];
            GColor color1 = colors[indices[n+1]];
            GColor color2 = colors[indices[n+2]];
            GColor color[3] = {color0, color1, color2};

            GPoint texs0 = texs[indices[n+0]];
            GPoint texs1 = texs[indices[n+1]];
            GPoint texs2 = texs[indices[n+2]];

            GMatrix T = GMatrix(texs1.x - texs0.x, texs2.x - texs0.x, texs0.x, texs1.y - texs0.y, texs2.y - texs0.y, texs0.y);
            GMatrix P = GMatrix(point1.x - point0.x, point2.x - point0.x, point0.x, point1.y - point0.y, point2.y - point0.y, point0.y);
            GMatrix invT = T.invert().value();
            GMatrix final = GMatrix::Concat(P, invT);

            ColorProxyShader *cs = new ColorProxyShader(points, color);
            TextureProxyShader *ts = new TextureProxyShader(paint.getShader(), final);
            BothProxyShader *bs = new BothProxyShader(cs, ts);
            GPaint newPaint = GPaint(bs);
            drawConvexPolygon(points, 3, newPaint);

        }
        else if (colors != NULL) {
            GColor color0 = colors[indices[n+0]];
            GColor color1 = colors[indices[n+1]];
            GColor color2 = colors[indices[n+2]];
            GColor color[] = {color0, color1, color2};

            ColorProxyShader *cs = new ColorProxyShader(points, color);
            GPaint newPaint = GPaint(cs);
            drawConvexPolygon(points, 3, newPaint);
        }

        else {
            GPoint texs0 = texs[indices[n+0]];
            GPoint texs1 = texs[indices[n+1]];
            GPoint texs2 = texs[indices[n+2]];
            GMatrix T = GMatrix(texs1.x - texs0.x, texs2.x - texs0.x, texs0.x, texs1.y - texs0.y, texs2.y - texs0.y, texs0.y);
            GMatrix P = GMatrix(point1.x - point0.x, point2.x - point0.x, point0.x, point1.y - point0.y, point2.y - point0.y, point0.y);
            GMatrix invT = T.invert().value();
            GMatrix final = GMatrix::Concat(P, invT);

            TextureProxyShader *ts = new TextureProxyShader(paint.getShader(), final);
            GPaint newPaint = GPaint(ts);
            drawConvexPolygon(points, 3, newPaint);
        }

        n = n+3;
    }
}

void MyCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint& paint){
    GPoint newVerts[4];
    GColor newColors[4];
    GPoint newTexs[4];
    GPoint *tex_pointer = nullptr;
    GColor *color_pointer = nullptr;

    int n = level + 1;
    float div = 1.0f/((float) n);
    int indices[] = {0, 1, 3, 1, 2, 3};

    for (int x=0; x<n; x++){
        float u = x * div;
        for (int y=0; y<n; y++){
            
            float v = y * div;

            newVerts[0] = makeNewP(verts, u, v);
            newVerts[1] = makeNewP(verts, u + div, v);
            newVerts[2] = makeNewP(verts, u + div, v + div);
            newVerts[3] = makeNewP(verts, u, v+div);

            if (colors) {
                newColors[0] = makeNewC(colors, u, v);
                newColors[1] = makeNewC(colors, u + div, v);
                newColors[2] = makeNewC(colors, u + div, v + div);
                newColors[3] = makeNewC(colors, u, v+div);
                color_pointer = newColors;
            }

            if (texs) {
                newTexs[0] = makeNewP(texs, u, v);
                newTexs[1] = makeNewP(texs, u + div, v);
                newTexs[2] = makeNewP(texs, u + div, v + div);
                newTexs[3] = makeNewP(texs, u, v+div);
                tex_pointer = newTexs;
            }

            drawMesh(newVerts, color_pointer, tex_pointer, 2, indices, paint);

        }
    }
}

GColor MyCanvas::makeNewC(const GColor colors[4], float u, float v){
    return (1.0f - u)*(1.0f - v)*colors[0] + u*(1.0f-v)*colors[1] + u*v*colors[2] + (1.0f-u)*v*colors[3];
}

GPoint MyCanvas::makeNewP(const GPoint points[4], float u, float v){
    return (1.0f - u)*(1.0f - v)*points[0] + u*(1.0f-v)*points[1] + u*v*points[2] + (1.0f-u)*v*points[3];
}


std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}



std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // // as fancy as you like
    // // ...
    // GColor white =  GColor::RGBA(1, 1, 1, 1);
    // canvas->clear(white);
    // // //canvas->fillRect(...);

    // GColor petalColor = GColor::RGBA(0.83f, 0.10f, 0.69f, 1);  //Pink
    // GColor centerColor = GColor::RGBA(0.93f, 0.78f, 0.04f, 1); // Yellow


    // // Draw center
    // GRect centerRect = GRect::XYWH(85, 85, 84, 84);
    // GRect petalL = GRect::XYWH(0, 85, 84, 84);
    // GRect petalT = GRect::XYWH(85, 0, 84, 84);
    // GRect petalR = GRect::XYWH(170, 85, 84, 84);
    // GRect petalD = GRect::XYWH(85, 170, 84, 84);
    // canvas->fillRect(centerRect, centerColor);
    // canvas->fillRect(petalL, petalColor);
    // canvas->fillRect(petalT, petalColor);
    // canvas->fillRect(petalR, petalColor);
    // canvas->fillRect(petalD, petalColor);

    return "PA6";
}