#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "SDL_helper.h"
#include "TestModel.h"


using std::vector;

using std::cos;
using std::sin;

using glm::vec3;
using glm::mat3;


struct Intersection
{
    vec3  position;
    float distance;
    int   triangle_index;
};

struct Camera {
    vec3  position;
    vec3  velocity;

    vec3  right;
    vec3  up;
    vec3  forward;

    float focal_length;
    float yaw;
    float pitch;
    float roll;
};


// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 100;
const int SCREEN_HEIGHT = 100;

vec3 light_position1 = vec3(0, -0.8, 0.8);
vec3 light_position = light_position1 /glm::length(light_position1);

vec3 light_color = 14.0f * vec3(1, 1, 1);

vec3 indirectLight = 0.5f * vec3(1, 1, 1);


// --------------------------------------------------------
// FUNCTION DECLARATIONS


void Update(float dt, Camera& camera);
void Draw(Window& window, const Camera& camera, const vector<Triangle>& triangles);
bool ClosestIntersection(vec3 start, vec3 direction, const vector<Triangle>& triangles, Intersection& closest_intersection);
vec3 DirectLight(const Triangle& triangle, vector<Triangle> triangles, const Intersection& intersection);


// --------------------------------------------------------
// FUNCTION DEFINITIONS

mat3 rotation_x(float theta) {
    mat3 rotation{
        {1, 0, 0},
        {0, cos(theta), -sin(theta)},
        {0, sin(theta),  cos(theta)}
    };
    return rotation;
}
mat3 rotation_y(float theta) {
    mat3 rotation{
        { cos(theta), 0, sin(theta)},
        {      0,     1,     0     },
        {-sin(theta), 0, cos(theta)}
    };
    return rotation;
}
mat3 rotation_z(float theta) {
    mat3 rotation{
        {cos(theta), -sin(theta), 0},
        {sin(theta),  cos(theta), 0},
        {     0,          0,      1},
    };
    return rotation;
}
mat3 rotation(float pitch, float yaw, float roll) {
    mat3 rotation = rotation_z(roll) * rotation_y(yaw) * rotation_x(pitch);
    return rotation;
}



int main(int argc, char* argv[])
{
    auto  W = float(SCREEN_WIDTH);
    auto  H = float(SCREEN_HEIGHT);
    float F = H / 2.0f;

    Window window = Window::Create("Lab1", SCREEN_WIDTH, SCREEN_HEIGHT);
    Clock  clock = Clock();
    Camera camera = Camera{
        vec3(0.0f, 0.0f, 2.0f),  // position
        vec3(0.0f, 0.0f, 2.0f),  // velocity
        vec3(1.0f, 0.0f, 0.0f),  // right
        vec3(0.0f, 1.0f, 0.0f),  // up
        vec3(0.0f, 0.0f, 1.0f),  // forward
        -F, 0.0f, 0.0f, 0.0f     // focal_length, yaw, pitch, roll
    };

    vector<Triangle> triangles = LoadTestModel();

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
        Update(dt, camera);
        Draw(window, camera, triangles);

        // NOTE: The pixels are not shown on the screen 
        // until we update the window with this method.
        window.update();
    }

    window.screenshot();

    Window::Destroy(&window);
    return 0;
}

void Update(float dt, Camera& camera)
{
    std::cout << "Render time: " << dt << " s." << std::endl;

    const Uint8* key_state = SDL_GetKeyboardState(nullptr);

    if (key_state[SDL_SCANCODE_UP]) { light_position.z -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_DOWN]) { light_position.z += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_LEFT]) { light_position.x -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_RIGHT]) { light_position.x += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_Z]) { light_position.y -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_C]) { light_position.y += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_RSHIFT]) { std::cout << "Pressing RSHIFT" << std::endl; }
    if (key_state[SDL_SCANCODE_RCTRL]) { std::cout << "Pressing RCTRL" << std::endl; }
    if (key_state[SDL_SCANCODE_W]) { camera.position.z -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_S]) { camera.position.z += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_D]) { camera.position.x += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_A]) { camera.position.x -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_E]) { camera.yaw += float(M_PI / 4.0) * dt; }
    if (key_state[SDL_SCANCODE_Q]) { camera.yaw -= float(M_PI / 4.0) * dt; }

    auto r = rotation(camera.pitch, camera.yaw, camera.roll);
    camera.right = vec3(r[0][0], r[0][1], r[0][2]);
    camera.up = vec3(r[1][0], r[1][1], r[1][2]);
    camera.forward = vec3(r[2][0], r[2][1], r[2][2]);

    /*      c0  c1  c2
     * r0   a   b   c
     * r1   d   e   f
     * r2   g   h   i
     *
     * right   = (a, d, g)
     * up      = (b, e, h)
     * forward = (c, f, i)
     *
     *  rotation      x
     * |a   b   c|   |1|   | a*1 + b*0 + c*0 |   | a |
     * |d   e   f| * |0| = | d*1 + e*0 + f*0 | = | d |
     * |g   h   i|   |0|   | g*1 + h*0 + i*0 |   | g |
     *
     * */
}

void Draw(Window& window, const Camera& camera, const vector<Triangle>& triangles)
{
    auto W = float(SCREEN_WIDTH);
    auto H = float(SCREEN_HEIGHT);

    Intersection closest_intersection = { };

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            float u = 2.0f * (float(x) / W) - 1.0f;  // Normalized between [-1, 1]
            float v = 2.0f * (float(y) / H) - 1.0f;  // Normalized between [-1, 1]

            auto direction = camera.right * u * (W / 2.0f) + camera.up * v * (H / 2.0f) + camera.forward * camera.focal_length;
            if (ClosestIntersection(camera.position, direction, triangles, closest_intersection))
            {
                auto triangle = triangles[closest_intersection.triangle_index];
                //                window.set_pixel(x, y, triangle.color);
                vec3 illumination = DirectLight(triangle, triangles, closest_intersection);
                vec3 R = triangle.color * (illumination + indirectLight);
                window.set_pixel(x, y, glm::clamp(R, BLACK, WHITE));
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
        vec3 b = s - v0;
        mat3 A(-d, e1, e2);
        vec3 x = glm::inverse(A) * b;

        // We know from the instructions that x = (t, u, v) where 0 <= t, 0 < u, 0 < v, u + v < 1
        // We name t tray instead since we already have a global variable t for time
        // To get the intersection we create an if-statement

        float t_ray = x[0];
        float u = x[1];
        float v = x[2];

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


vec3 DirectLight(const Triangle& triangle, vector<Triangle> triangles, const Intersection& intersection) {
    vec3  rh = light_position - intersection.position;
    float r = glm::length(rh);
    vec3  n = glm::normalize(triangle.normal);
    float A = float(4.0 * M_PI) * r * r;
    vec3  P = light_color;
    vec3  B = P / A;
    //vec3 phi = vec3(0.4, 0.4, 0);
    vec3 D = B * glm::max(glm::dot(rh, n), 0.0f);

    Intersection shadow;

    if (ClosestIntersection(light_position, -rh / r, triangles, shadow)) {
        /*std::cout << r;
        std::cout << "hej";
        std::cout << shadow.distance;*/
        // These values are very close and the if statement gets true too often so if we add values to the shadow distance side we get better results somehow
        if (shadow.distance + 0.000001 < r) {
            //std::cout << "hej";
            return vec3(0);
        }

    }

    vec3 R = triangle.color * D;

    return R;
}
