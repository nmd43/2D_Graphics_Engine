#include "include/GRect.h"
#include "include/GPath.h"
#include "starter.h"
#include "include/GMath.h"

GPoint makeGPoint(float x, float y){
    GPoint point;
    point.x = x;
    point.y = y;
    return point;
}

void GPath::addRect(const GRect& r, Direction dir){
    this -> moveTo(makeGPoint(r.left, r.top));

    if (dir == Direction::kCW_Direction) {
        this->lineTo(makeGPoint(r.right, r.top));
        this->lineTo(makeGPoint(r.right, r.bottom));
        this->lineTo(makeGPoint(r.left, r.bottom));
    }
    else{
        this->lineTo(makeGPoint(r.left, r.bottom));
        this->lineTo(makeGPoint(r.right, r.bottom));
        this->lineTo(makeGPoint(r.right, r.top));
    }
}

void GPath::addPolygon(const GPoint pts[], int count){

    if (count < 3) {
        return;
    }

    this->moveTo(pts[0]);

    for (int i = 1; i<count; i++){
        this->lineTo(pts[i]);
    }

}

void GPath::addCircle(GPoint center, float radius, GPath::Direction direction)
{
    const float SQRT = 0.70710678118f * radius;
    const float TAN = 0.41421356237f * radius;

    if (direction == GPath::Direction::kCCW_Direction)
    {
        this->moveTo({center.x + radius, center.y});
        this->quadTo({center.x + radius, center.y - TAN}, {center.x + SQRT, center.y - SQRT});
        this->quadTo({center.x + TAN, center.y - radius}, {center.x, center.y - radius});
        this->quadTo({center.x - TAN, center.y - radius}, {center.x - SQRT, center.y - SQRT});
        this->quadTo({center.x - radius, center.y - TAN}, {center.x - radius, center.y});
        this->quadTo({center.x - radius, center.y + TAN}, {center.x - SQRT, center.y + SQRT});
        this->quadTo({center.x - TAN, center.y + radius}, {center.x, center.y + radius});
        this->quadTo({center.x + TAN, center.y + radius}, {center.x + SQRT, center.y + SQRT});
        this->quadTo({center.x + radius, center.y + TAN}, {center.x + radius, center.y});
    }
    else
    { // CCW direction
        this->moveTo({center.x + radius, center.y});
        this->quadTo({center.x + radius, center.y + TAN}, {center.x + SQRT, center.y + SQRT});
        this->quadTo({center.x + TAN, center.y + radius}, {center.x, center.y + radius});
        this->quadTo({center.x - TAN, center.y + radius}, {center.x - SQRT, center.y + SQRT});
        this->quadTo({center.x - radius, center.y + TAN}, {center.x - radius, center.y});
        this->quadTo({center.x - radius, center.y - TAN}, {center.x - SQRT, center.y - SQRT});
        this->quadTo({center.x - TAN, center.y - radius}, {center.x, center.y - radius});
        this->quadTo({center.x + TAN, center.y - radius}, {center.x + SQRT, center.y - SQRT});
        this->quadTo({center.x + radius, center.y - TAN}, {center.x + radius, center.y});
    }
}

static inline GPoint bezierQuadPoint(GPoint A, GPoint B, GPoint C, float t) {
    GPoint result;
    result.x = (t*t)*(A.x - 2*B.x + C.x) + t*(-2*A.x + 2*B.x) + A.x;
    result.y = (t*t)*(A.y - 2*B.y + C.y) + t*(-2*A.y + 2*B.y) + A.y;
    return result;
}

static inline GPoint bezierCubicPoint(GPoint A, GPoint B, GPoint C, GPoint D, float t) {
    GPoint result;
    result.x = (t*t*t)*(-A.x + 3*B.x - 3*C.x + D.x) + (t*t)*(3*A.x - 6*B.x + 3*C.x) + t*(-3*A.x + 3*B.x) + A.x;
    result.y = (t*t*t)*(-A.y + 3*B.y - 3*C.y + D.y) + (t*t)*(3*A.y - 6*B.y + 3*C.y) + t*(-3*A.y + 3*B.y) + A.y;
    return result;
}

static inline std::vector<float> solveQuadratic(float a, float b, float c){
    float discriminant = b*b - 4*a*c;
    std::vector<float> roots;
    float t;
    if (discriminant > 0) {
        t = ((-b + sqrt(discriminant)) / (2*a));
        if (t > 0 && t < 1) roots.push_back(t);
        t = ((-b - sqrt(discriminant)) / (2*a));
        if (t > 0 && t < 1) roots.push_back(t);
    }

    else if (discriminant == 0) {
        t = (-b / (2*a));
        if (t > 0 && t < 1) roots.push_back(t);
    }

    return roots;
}

static inline GRect quadraticBoundingBox(GPoint A, GPoint B, GPoint C){
    float denomX = (A.x - 2*B.x + C.x);
    float denomY = (A.y - 2*B.y + C.y);
    float tX = (A.x - B.x) / denomX;
    float tY = (A.y - B.y) / denomY;
    std::vector<GPoint> quad_points;
    quad_points.push_back(C);
    quad_points.push_back((0.5f*0.5f)*(A + 2*B + C));

    // Check if t values are within [0, 1] and add them to points if they are
    if (tX > 0 && tX < 1 && denomX != 0) quad_points.push_back(bezierQuadPoint(A, B, C, tX));
    if (tY > 0 && tY < 1 && denomY != 0) quad_points.push_back(bezierQuadPoint(A, B, C, tY));

    float minX = quad_points[0].x;
    float minY = quad_points[0].y;
    float maxX = quad_points[0].x;
    float maxY = quad_points[0].y;
    for (const auto& point : quad_points) {
        minX = std::min(minX, point.x);
        minY = std::min(minY, point.y);
        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);
    }
    return GRect::LTRB(minX, minY, maxX, maxY);
}

static inline GRect cubicBoundingBox(GPoint A, GPoint B, GPoint C, GPoint D){
    std::vector<GPoint> cubic_points;
    cubic_points.push_back(D);
    cubic_points.push_back((0.5f*0.5f*0.5f)*(A + 3*B + 3*C + D));

    float aX = -A.x + 3*B.x - 3*C.x + D.x;
    float bX = 2*A.x -4*B.x + 2*C.x;
    float cX = B.x - A.x;
    float aY = -A.y + 3*B.y - 3*C.y + D.y;
    float bY = 2*A.y -4*B.y + 2*C.y;
    float cY = B.y - A.y;

    std::vector<float> tX = solveQuadratic(aX, bX, cX);
    std::vector<float> tY = solveQuadratic(aY, bY, cY);

    for (float i : tX) {
        cubic_points.push_back(bezierCubicPoint(A, B, C, D, i));
    }

    for (float j : tY) {
        cubic_points.push_back(bezierCubicPoint(A, B, C, D, j));
    }

    float minX = cubic_points[0].x;
    float minY = cubic_points[0].y;
    float maxX = cubic_points[0].x;
    float maxY = cubic_points[0].y;
    for (const auto& point : cubic_points) {
        minX = std::min(minX, point.x);
        minY = std::min(minY, point.y);
        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);
    }
    return GRect::LTRB(minX, minY, maxX, maxY);
}

GRect GPath::bounds() const{

    if (fPts.empty()){
        return GRect::WH(0,0);
    }

    float minX = fPts[0].x;
    float minY = fPts[0].y;
    float maxX = fPts[0].x;
    float maxY = fPts[0].y;

    Edger edger = Edger(*this);
    GPoint points[GPath::kMaxNextPoints];
    while (auto verb = edger.next(points)) {
        if (verb == GPath::kLine) {
            minX = std::min(minX, std::min(points[0].x, points[1].x));
            minY = std::min(minY, std::min(points[0].y, points[1].y));
            maxX = std::max(maxX, std::max(points[0].x, points[1].x));
            maxY = std::max(maxY, std::max(points[0].y, points[1].y));
        }
        else if (verb == GPath::kQuad) {
            GRect boundary = quadraticBoundingBox(points[0], points[1], points[2]);
            minX = std::min(minX, boundary.left);
            minY = std::min(minY, boundary.top);
            maxX = std::max(maxX, boundary.right);
            maxY = std::max(maxY, boundary.bottom); 
        }
        else if (verb == GPath::kCubic) {
            GRect boundary = cubicBoundingBox(points[0], points[1], points[2], points[3]);
            minX = std::min(minX, boundary.left);
            minY = std::min(minY, boundary.top);
            maxX = std::max(maxX, boundary.right);
            maxY = std::max(maxY, boundary.bottom); 
        }
    }

    return GRect::LTRB(minX, minY, maxX, maxY);
}

void GPath::transform(const GMatrix& m){

    m.mapPoints(fPts.data(), fPts.data(), fPts.size());

}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t){
    GPoint p0 = src[0];
    GPoint p1 = src[1];
    GPoint p2 = src[2];

    dst[0] = p0;
    dst[4] = p2;

    GPoint dst_1 = (t*(p1 - p0)) + p0;
    GPoint dst_3 = (t*(p2 - p1)) + p1;

    dst[1] = dst_1;
    dst[3] = dst_3;
    dst[2] = (t*(dst_3 - dst_1)) + dst_1;

}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t){

    GPoint p0 = src[0];
    GPoint p1 = src[1];
    GPoint p2 = src[2];
    GPoint p3 = src[3];

    dst[0] = p0;
    dst[6] = p3;

    GPoint dst_1 = (t*(p1 - p0)) + p0;
    GPoint mid = (t*(p2 - p1)) + p1;
    GPoint dst_5 = (t*(p3 - p2)) + p2;

    dst[1] = dst_1;
    dst[5] = dst_5;

    GPoint dst_2 = (t*(mid - dst_1)) + dst_1;
    GPoint dst_4 = (t*(dst_5 - mid)) + mid;

    dst[2] = dst_2;
    dst[4] = dst_4;

    dst[3] = (t*(dst_4 - dst_2)) + dst_2;
}