#include "test.h"
#include <vector>


float lerp1(float a, float b, float t) {
    return (1.0f - t) * a  +  t * b;
}
float lerp2(float a, float b, float t)
{
    return a + t * (b - a);
}
float lerp3(float a, float b, float t)
{
    return b - (b - a) * (1.0f - t);
}


Test(Lerp0)
{
    float start = 0.0f;
    float stop  = 499.0f;
    float delta = stop - start;
    int   n     = int(std::abs(delta)) + 1;

    std::vector<float> result;
    result.reserve(500);

    float step = delta / float(n-1);
    float current = start;
    for (int i = 0; i < n; ++i) {
        result.push_back(current);
        current += step;
    }

    for (int i = 0; i < n; ++i) {
        Checkf(int(result[i]), ==, i, "%f != %f", result[i], float(i));
    }
}

Test(Lerp1)
{
    float start = 0.0f;
    float stop  = 499.0f;
    float delta = stop - start;
    int   n     = int(std::abs(delta)) + 1;

    std::vector<float> result;
    result.reserve(500);
    
    for (int i = 0; i < n; ++i) {
        float t = float(i) / float(n-1);  // Make `t` go from 0 to 1.
        result.push_back(lerp1(start, stop, t));
    }

    for (int i = 0; i < n; ++i) {
        Checkf(int(result[i]), ==, i, "%f != %f", result[i], float(i));
    }
}

Test(Lerp2)
{
    float start = 0.0f;
    float stop  = 499.0f;
    float delta = stop - start;
    int   n     = int(std::abs(delta)) + 1;

    std::vector<float> result;
    result.reserve(500);

    for (int i = 0; i < n; ++i) {
        float t = float(i) / float(n-1);  // Make `t` go from 0 to 1.
        result.push_back(lerp2(start, stop, t));
    }

    for (int i = 0; i < n; ++i) {
        Checkf(int(result[i]), ==, i, "%f != %f", result[i], float(i));
    }
}

Test(Lerp3)
{
    float start = 0.0f;
    float stop  = 499.0f;
    float delta = stop - start;
    int   n     = int(std::abs(delta)) + 1;

    std::vector<float> result;
    result.reserve(500);

    for (int i = 0; i < n; ++i) {
        float t = float(i) / float(n-1);  // Make `t` go from 0 to 1.
        result.push_back(lerp3(start, stop, t));
    }

    for (int i = 0; i < n; ++i) {
        Checkf(int(result[i]), ==, i, "%f != %f", result[i], float(i));
    }
}



int main()
{
    RunAllTests();
}