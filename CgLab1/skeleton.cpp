// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include "SDL.h"
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDLauxiliary.h"

using glm::vec3;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int t;
bool isRainbow = false;

// --------------------------------------------------------
// FUNCTION DECLARATIONS

void DrawRainbow(SDL_Surface* screen);
void DrawStarField(SDL_Surface* screen, const std::vector<vec3>& stars);
void UpdateStarField(std::vector<vec3>& stars, vec3 velocity);

// --------------------------------------------------------
// FUNCTION DEFINITIONS

template <typename T>
T lerp(const T& start, const T& stop, float t) {
    // t is a value between 0 - 1
    // t = 0 returns start
    // t = 1 returns stop
    // Otherwise, return in between
    return start * (1.0f - t) + stop * t;
}

template <typename T>
std::vector<T> Interpolate(const T& start, const T& stop, int samples) {
    std::vector<T> result;
    result.reserve(samples);

    if (samples == 1) {
        result.push_back(start);
        return result;
    }

    auto dividend = float(samples);
    for (int i = 0; i < samples; ++i) {
        auto value = lerp(start, stop, float(i) / dividend);
        result.push_back(value);
    }

    return result;
}

vec3 Random() {
    float x = 2.0f * (float(rand()) / float(RAND_MAX)) - 1.0f;
    float y = 2.0f * (float(rand()) / float(RAND_MAX)) - 1.0f;
    float z = float(rand()) / float(RAND_MAX);
    return vec3(x, y, z);
}

int main(int argc, char* argv[])
{
    std::vector<vec3> stars(1000);
    t = SDL_GetTicks();
    for (int i = 0; i < stars.size(); ++i) {
        stars[i] = Random();
    }

    SDL_Surface* screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
    while (NoQuitMessageSDL())
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    isRainbow != isRainbow;
                }
            }
        }
        if (isRainbow) {
            DrawRainbow(screen);
        }
        else {
            UpdateStarField(stars, vec3(0, 0, 0.0001));
            DrawStarField(screen, stars);
        }
        

        // DrawRainbow(screen);

        if (SDL_MUSTLOCK(screen))
            SDL_UnlockSurface(screen);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    SDL_SaveBMP(screen, "../" __TIME__ "screenshot.bmp");
    return 0;
}

void UpdateStarField(std::vector<vec3>& stars, vec3 velocity) {

    int t2 = SDL_GetTicks();
    float dt = float(t2 - t);
    t = t2;
    for (int i = 0; i < stars.size(); ++i) {

        stars[i] = stars[i] - (velocity * dt);

        if (stars[i].z <= 0)
            stars[i].z += 1;
        if (stars[i].z > 1)
            stars[i].z -= 1;
    }
}

void DrawStarField(SDL_Surface* screen, const std::vector<vec3>& stars) {
    /*for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            PutPixelSDL(screen, x, y, vec3(0.0, 0.0, 0.0));*/

    SDL_FillRect(screen, 0, 0); //Given in the instructions

    float f = SCREEN_HEIGHT / 2.0f; //Given in the instructions
    for (int i = 0; i < stars.size(); ++i) {
        auto position = stars[i];

        auto u = int(f * position.x / position.z + SCREEN_WIDTH / 2.0f);
        auto v = int(f * position.y / position.z + SCREEN_HEIGHT / 2.0f);
        vec3 color = 0.2f * vec3(1, 1, 1) / (stars[i].z * stars[i].z); //Given in the instructions
        PutPixelSDL(screen, u, v, color);
    }
}


void DrawRainbow(SDL_Surface* screen)
{
    vec3 RED(1, 0, 0);    // red
    vec3 BLUE(0, 0, 1);   // blue
    vec3 GREEN(0, 1, 0);  // green
    vec3 YELLOW(1, 1, 0); // yellow

    auto left = Interpolate(RED, GREEN, SCREEN_HEIGHT);
    auto right = Interpolate(BLUE, YELLOW, SCREEN_HEIGHT);

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        auto colors = Interpolate(left[y], right[y], SCREEN_WIDTH);
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            PutPixelSDL(screen, x, y, colors[x]);
        }
    }
}


