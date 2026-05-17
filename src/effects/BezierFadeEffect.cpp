#include "BezierFadeEffect.h"
#include "huestream/common/data/Color.h"
#include <cmath>

using namespace huestream;

struct BezierCurve {
    float x1;
    float y1;
    float x2;
    float y2;
};

float bezierX(float t, float x1, float x2) {
    return 3 * pow((1 - t), 2) * t * x1 + 3 * (1 - t) * pow(t, 2) * x2 +
           pow(t, 3);
}

float bezierY(float u, float y1, float y2) {
    return 3 * pow((1 - u), 2) * u * y1 + 3 * (1 - u) * pow(u, 2) * y2 +
           pow(u, 3);
}

float solveBezier(float t, BezierCurve curve) {
    // using the binary search
    int steps = 20;
    float low = 0;
    float high = 1;
    float mid = high / 2;

    for (int i = 0; i <= steps; i++) {
        mid = (low + high) / 2;
        float x = bezierX(mid, curve.x1, curve.x2);
        if (x < t) {
            low = mid;
        } else {
            high = mid;
        }
    }

    float intensity = bezierY(mid, curve.y1, curve.y2);
    return intensity;
}

huestream::Color BezierFadeEffect::compute(float t) const {
    // NOTE: https://easings.net/#
    BezierCurve easeOut;
    easeOut.x1 = 0.83;
    easeOut.y1 = 0.00;
    easeOut.x2 = 0.17;
    easeOut.y2 = 1.00;

    Color baseColor(1.0, 1.0, 1.0);
    double intensity = solveBezier(1.0f - t, easeOut);

    Color computedColor(baseColor.GetR() * intensity,
                        baseColor.GetG() * intensity,
                        baseColor.GetB() * intensity);

    return computedColor;
}
