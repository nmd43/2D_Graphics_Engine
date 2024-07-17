/*
 *  Copyright 2015 Mike Reed
 */

#ifndef GPath_DEFINED
#define GPath_DEFINED

#include <vector>
#include "GMatrix.h"
#include "GPoint.h"
#include "GRect.h"

class GPath {
public:
    GPath();
    ~GPath();

    GPath& operator=(const GPath&);

    /**
     *  Erase any previously added points/verbs, restoring the path to its initial empty state.
     */
    void reset();

    /**
     *  Start a new contour at the specified coordinate.
     *  Returns a reference to this path.
     */
    void moveTo(GPoint p) {
        fPts.push_back(p);
        fVbs.push_back(kMove);
    }
    void moveTo(float x, float y) { this->moveTo({x, y}); }

    /**
     *  Connect the previous point (either from a moveTo or lineTo) with a line segment to
     *  the specified coordinate.
     *  Returns a reference to this path.
     */
    void lineTo(GPoint p) {
        assert(fVbs.size() > 0);
        fPts.push_back(p);
        fVbs.push_back(kLine);
    }
    void lineTo(float x, float y) { this->lineTo({x, y}); }

    /**
     *  Connect the previous point with a quadratic bezier to the specified coordinates.
     */
    void quadTo(GPoint, GPoint);
    void quadTo(float x1, float y1, float x2, float y2) {
        this->quadTo({x1, y1}, {x2, y2});
    }

    /**
     *  Connect the previous point with a cubic bezier to the specified coordinates.
     */
    void cubicTo(GPoint, GPoint, GPoint);
    void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
        this->cubicTo({x1, y1}, {x2, y2}, {x3, y3});
    }

    enum Direction {
        kCW_Direction,  // clockwise
        kCCW_Direction, // counter-clockwise
    };
    
    /**
     *  Append a new contour to this path, made up of the 4 points of the specified rect,
     *  in the specified direction.
     *
     *  In either direction the contour must begin at the top-left corner of the rect.
     */
    void addRect(const GRect&, Direction = kCW_Direction);

    /**
     *  Append a new contour to this path with the specified polygon.
     *  Calling this is equivalent to calling moveTo(pts[0]), lineTo(pts[1..count-1]).
     */
    void addPolygon(const GPoint pts[], int count);

    /**
     *  Append a new contour respecting the Direction. The contour should be an approximate
     *  circle (8 quadratic curves will suffice) with the specified center and radius.
     */
    void addCircle(GPoint center, float radius, Direction = kCW_Direction);

    int countPoints() const { return (int)fPts.size(); }

    /**
     *  Return the tight bounds of all of the curve and line segments in the path.
     *  Curve segments may need to be chopped at X and Y extrema to compute this correctly.
     *
     *  If there are no points, returns an empty rect (all zeros)
     */
    GRect bounds() const;

    /**
     *  Transform the path in-place by the specified matrix.
     */
    void transform(const GMatrix&);

    void offset(float dx, float dy) {
        this->transform(GMatrix::Translate(dx, dy));
    }

    enum Verb {
        kMove,  // returns pts[0] from Iter
        kLine,  // returns pts[0]..pts[1] from Iter and Edger
        kQuad,  // returns pts[0]..pts[2] from Iter and Edger
        kCubic, // returns pts[0]..pts[3] from Iter and Edger
    };

    // maximum number of points returned by Iter::next() and Edger::next()
    enum {
        kMaxNextPoints = 4
    };

    /**
     *  Walks the path, returning each verb that was entered.
     *  e.g.    moveTo() returns kMove
     *          lineTo() returns kLine
     */
    class Iter {
    public:
        Iter(const GPath&);
        std::optional<Verb> next(GPoint pts[]);

    private:
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
    };

    /**
     *  Walks the path, returning "edges" only. Thus it does not return kMove, but will return
     *  the final closing "edge" for each contour.
     *
     * Typical calling pattern...
     *
     *   GPoint pts[GPath::kMaxNextPoints];
     *   GPath::Edger edger(path);
     *   while (auto v = edger.next(pts)) {
     *       switch (v.value()) {
     *           case GPath::kLine: // lpts[0..1]
     *           case GPath::kQuad: // pts[0..2]
     *           case GPath::kCubic: // pts[0..3]
     *  }
     */
    class Edger {
    public:
        Edger(const GPath&);
        std::optional<Verb> next(GPoint pts[]);

    private:
        const GPoint* fPrevMove;
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
        int fPrevVerb;
    };

    /**
     *  Given 0 < t < 1, subdivide the src[] quadratic bezier at t into two new quadratics in dst[]
     *  such that
     *  0...t is stored in dst[0..2]
     *  t...1 is stored in dst[2..4]
     */
    static void ChopQuadAt(const GPoint src[3], GPoint dst[5], float t);

    /**
     *  Given 0 < t < 1, subdivide the src[] cubic bezier at t into two new cubics in dst[]
     *  such that
     *  0...t is stored in dst[0..3]
     *  t...1 is stored in dst[3..6]
     */
    static void ChopCubicAt(const GPoint src[4], GPoint dst[7], float t);

    void dump() const;

private:
    std::vector<GPoint> fPts;
    std::vector<Verb>   fVbs;
};

#endif

