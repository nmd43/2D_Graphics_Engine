/*
 *  Copyright 2018 Mike Reed
 */

#include "../include/GPath.h"
#include "../include/GMatrix.h"

GPath::GPath() {}
GPath::~GPath() {}

GPath& GPath::operator=(const GPath& src) {
    if (this != &src) {
        fPts = src.fPts;
        fVbs = src.fVbs;
    }
    return *this;
}

void GPath::reset() {
    fPts.clear();
    fVbs.clear();
}

void GPath::dump() const {
    Iter iter(*this);
    GPoint pts[GPath::kMaxNextPoints];
    while (auto v = iter.next(pts)) {
        switch (*v) {
            case kMove:
                printf("M %g %g\n", pts[0].x, pts[0].y);
                break;
            case kLine:
                printf("L %g %g\n", pts[1].x, pts[1].y);
                break;
            case kQuad:
                printf("Q %g %g  %g %g\n", pts[1].x, pts[1].y, pts[2].x, pts[2].y);
                break;
            case kCubic:
                printf("C %g %g  %g %g  %g %g\n",
                       pts[1].x, pts[1].y,
                       pts[2].x, pts[2].y,
                       pts[3].x, pts[3].y);
                break;
        }
    }
}

void GPath::quadTo(GPoint p1, GPoint p2) {
    assert(fVbs.size() > 0);
    fPts.push_back(p1);
    fPts.push_back(p2);
    fVbs.push_back(kQuad);
}

void GPath::cubicTo(GPoint p1, GPoint p2, GPoint p3) {
    assert(fVbs.size() > 0);
    fPts.push_back(p1);
    fPts.push_back(p2);
    fPts.push_back(p3);
    fVbs.push_back(kCubic);
}

/////////////////////////////////////////////////////////////////

GPath::Iter::Iter(const GPath& path) {
    fCurrPt = path.fPts.data();
    fCurrVb = path.fVbs.data();
    fStopVb = fCurrVb + path.fVbs.size();
}

std::optional<GPath::Verb> GPath::Iter::next(GPoint pts[]) {
    assert(fCurrVb <= fStopVb);
    if (fCurrVb == fStopVb) {
        return {};
    }
    Verb v = *fCurrVb++;
    switch (v) {
        case kMove:
            pts[0] = *fCurrPt++;
            break;
        case kLine:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            break;
        case kQuad:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            pts[2] = *fCurrPt++;
            break;
        case kCubic:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            pts[2] = *fCurrPt++;
            pts[3] = *fCurrPt++;
            break;
    }
    return v;
}

constexpr int kDoneVerb = -1;

GPath::Edger::Edger(const GPath& path) {
    fPrevMove = nullptr;
    fCurrPt = path.fPts.data();
    fCurrVb = path.fVbs.data();
    fStopVb = fCurrVb + path.fVbs.size();
    fPrevVerb = kDoneVerb;
}

std::optional<GPath::Verb> GPath::Edger::next(GPoint pts[]) {
    assert(fCurrVb <= fStopVb);
    bool do_return = false;
    while (fCurrVb < fStopVb) {
        switch (*fCurrVb++) {
            case kMove:
                if (fPrevVerb == kLine) {
                    pts[0] = fCurrPt[-1];
                    pts[1] = *fPrevMove;
                    do_return = true;
                }
                fPrevMove = fCurrPt++;
                fPrevVerb = kMove;
                break;
            case kLine:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                fPrevVerb = kLine;
                return kLine;
            case kQuad:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                pts[2] = *fCurrPt++;
                fPrevVerb = kQuad;
                return kQuad;
            case kCubic:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                pts[2] = *fCurrPt++;
                pts[3] = *fCurrPt++;
                fPrevVerb = kCubic;
                return kCubic;
        }
        if (do_return) {
            return kLine;
        }
    }
    if (fPrevVerb >= kLine && fPrevVerb <= kCubic) {
        pts[0] = fCurrPt[-1];
        pts[1] = *fPrevMove;
        fPrevVerb = kDoneVerb;
        return kLine;
    } else {
        return {};
    }
}
