// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "SDL_helper.h"


using glm::vec3;

// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;


// --------------------------------------------------------
// FUNCTION DECLARATIONS

void UpdateStarField(std::vector<vec3>& stars, vec3 velocity, float dt);
void DrawStarField(Window& window, const std::vector<vec3>& stars);
void DrawRainbow(Window& window);
vec3 Random();


// --------------------------------------------------------
// FUNCTION DEFINITIONS

template <typename T>
T lerp(const T& start, const T& stop, float t)
{
    // t is a value between 0 - 1
    // t = 0 returns start
    // t = 1 returns stop
    // Otherwise, return in between
    return start * (1.0f - t) + stop * t;
}

template <typename T>
std::vector<T> Interpolate(const T& start, const T& stop, int samples)
{
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

vec3 Random()
{
    float x = 2.0f * (float(rand()) / float(RAND_MAX)) - 1.0f;
    float y = 2.0f * (float(rand()) / float(RAND_MAX)) - 1.0f;
    float z = float(rand()) / float(RAND_MAX);
    return vec3(x, y, z);
}

int main(int argc, char* argv[])
{
    std::vector<vec3> stars(1000);
    for (auto& star : stars)
        star = Random();

    Window window = Window::Create("Lab1", SCREEN_WIDTH, SCREEN_HEIGHT);
    Clock  clock  = Clock();

    bool show_rainbow = true;
    bool is_running   = true;
    while (is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)  is_running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) show_rainbow = !show_rainbow;
        }

        float dt = clock.tick();

        if (show_rainbow)
        {
            DrawRainbow(window);
        }
        else
        {
            UpdateStarField(stars, vec3(0, 0, 1), dt);
            DrawStarField(window, stars);
        }

        // NOTE: The pixels are not shown on the screen
        // until we update the window with this method.
        window.update();
    }

    window.screenshot();

    Window::Destroy(&window);
    return 0;
}

void UpdateStarField(std::vector<vec3>& stars, vec3 velocity, float dt)
{
    for (auto& star : stars)
    {
        star = star - (velocity * dt);

        if (star.z <= 0)
            star.z += 1;
        if (star.z > 1)
            star.z -= 1;
    }
}

void DrawStarField(Window& window, const std::vector<vec3>& stars)
{
    window.fill(BLACK);

    float f = SCREEN_HEIGHT / 2.0f; //Given in the instructions
    for (const auto& star : stars)
    {
        auto position = star;

        auto u = int(f * position.x / position.z + SCREEN_WIDTH  / 2.0f);
        auto v = int(f * position.y / position.z + SCREEN_HEIGHT / 2.0f);

        if (0 <= u && u < SCREEN_WIDTH && 0 <= v && v < SCREEN_HEIGHT)
        {
            vec3 color = 0.2f * vec3(1, 1, 1) / (star.z * star.z);
            window.set_pixel(u, v, glm::clamp(color, BLACK, WHITE));
        }
    }
}


void DrawRainbow(Window& window)
{
    auto left  = Interpolate(RED,  GREEN,  SCREEN_HEIGHT);
    auto right = Interpolate(BLUE, YELLOW, SCREEN_HEIGHT);

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        auto colors = Interpolate(left[y], right[y], SCREEN_WIDTH);
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            window.set_pixel(x, y, colors[x]);
        }
    }
}