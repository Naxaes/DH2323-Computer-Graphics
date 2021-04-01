#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "SDL_helper.h"
#include "TestModel.h"


using std::vector;

using glm::vec3;
using glm::mat3;


struct Intersection
{
    vec3  position;
    float distance;
    int   triangle_index;
};


// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH  = 100;
const int SCREEN_HEIGHT = 100;


// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Update(float dt, vec3& camera);
void Draw(Window& window, vec3 camera_position, const vector<Triangle>& triangles);
bool ClosestIntersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closest_intersection);



// --------------------------------------------------------
// FUNCTION DEFINITIONS

int main(int argc, char* argv[])
{
    Window window = Window::Create("Lab1", SCREEN_WIDTH, SCREEN_HEIGHT);
    Clock  clock  = Clock();

    vector<Triangle> triangles = LoadTestModel();
    vec3 camera_position(0.0f, 0.0f, 2.0f);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

        }

        float dt = clock.tick();
        Update(dt, camera_position);
        Draw(window, camera_position, triangles);

        // NOTE: The pixels are not shown on the screen 
        // until we update the window with this method.
        window.update();
    }

    window.screenshot();

    Window::Destroy(&window);
    return 0;
}

void Update(float dt, vec3& camera)
{
    std::cout << "Render time: " << dt << " s." << std::endl;

	const Uint8* key_state = SDL_GetKeyboardState(nullptr);

	if (key_state[SDL_SCANCODE_UP])     { camera.z -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_DOWN])   { camera.z += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_LEFT])   { camera.x -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_RIGHT])  { camera.x += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_RSHIFT]) { std::cout << "Pressing RSHIFT" << std::endl; }
	if (key_state[SDL_SCANCODE_RCTRL])  { std::cout << "Pressing RCTRL"	<< std::endl; }
	if (key_state[SDL_SCANCODE_W])      { std::cout << "Pressing W"		<< std::endl; }
	if (key_state[SDL_SCANCODE_S])      { std::cout << "Pressing S"		<< std::endl; }
	if (key_state[SDL_SCANCODE_D])      { std::cout << "Pressing D"		<< std::endl; }
	if (key_state[SDL_SCANCODE_A])      { std::cout << "Pressing A"		<< std::endl; }
	if (key_state[SDL_SCANCODE_E])      { std::cout << "Pressing E"		<< std::endl; }
	if (key_state[SDL_SCANCODE_Q])      { std::cout << "Pressing Q"		<< std::endl; }
}

void Draw(Window& window, vec3 camera_position, const vector<Triangle>& triangles)
{
    auto W = float(SCREEN_WIDTH);
    auto H = float(SCREEN_HEIGHT);

    float F = H / 2.0f;
    Intersection closest_intersection = { };

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            auto direction = glm::normalize(vec3(x - W/2.0, y - H/2.0, -F));
            if (ClosestIntersection(camera_position, direction, triangles, closest_intersection))
            {
                auto triangle = triangles[closest_intersection.triangle_index];
                window.set_pixel(x, y, triangle.color);
            }
            else
            {
                window.set_pixel(x, y, BLACK);
            }
        }
    }
}

bool ClosestIntersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closest_intersection)
{
    float min_distance = std::numeric_limits<float>::max();
    vec3  s = start;
    vec3  d = direction;

    for (int i = 0; i < triangles.size(); ++i)
    {
        Triangle triangle = triangles[i];
        vec3 v0 = triangle.v0;
        vec3 v1 = triangle.v1;
        vec3 v2 = triangle.v2;
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 b  = s - v0;
        mat3 A(-d, e1, e2);
        vec3 x = glm::inverse(A) * b;

        // We know from the instructions that x = (t, u, v) where 0 <= t, 0 < u, 0 < v, u + v < 1
        // We name t tray instead since we already have a global variable t for time
        // To get the intersection we create an if-statement

        float t_ray = x[0];
        float u     = x[1];
        float v     = x[2];

        if (t_ray >= 0 && u >= 0 && v >= 0 && u + v <= 1)
        {
            vec3 pos = start + direction * t_ray;
            if (t_ray < min_distance)
            {
                min_distance = t_ray;
                closest_intersection.position = pos;
                closest_intersection.distance = t_ray;
                closest_intersection.triangle_index = i;
            }
        }
    }

    if (min_distance < std::numeric_limits<float>::max())
        return true;
    else
        return false;
}