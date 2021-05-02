#include <iostream>
#include <vector>
#include <tuple>

#include "glm/glm.hpp"
#include "SDL_helper.h"
#include "TestModel.h"
#include <algorithm>


using std::vector;
using std::tuple;

using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::ivec2;

using glm::max;


const int SCREEN_WIDTH  = 500;
const int SCREEN_HEIGHT = 500;


vec3 light_position = vec3(0, -0.5, -0.7);
const vec3 light_power    = 10.1f * vec3(1, 1, 1);
const vec3 indirect_light_power_per_area = 0.5f * vec3(1, 1, 1);


struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 reflectance;
};


struct Pixel {
    int x;
    int y;
    float z_inv;
    vec3  illumination;
};


struct Camera {
	vec3  position;

	vec3  right;
	vec3  up;
	vec3  forward;

    mat3  transform;

	float focal_length;
	float yaw;
	float pitch;
	float roll;

};

float depth_buffer[SCREEN_HEIGHT][SCREEN_WIDTH] = { 0 };


// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw(Window& window, const Camera& camera, const vector<Triangle>& triangles);
void Update(Camera& camera, float dt);

vector<Pixel> Interpolate(Pixel a, Pixel b);
Pixel VertexShader(const Camera& camera, const Vertex& v);
vector<Pixel> Rasterize(const vector<Pixel>& polygon);
void PixelShader(Window& window, const Pixel& pixel, const Triangle& triangle);

void DrawLine(Window& window, Pixel a, Pixel b, vec3 color);
void DrawPolygonEdges(Window& window, const vector<vec3>& vertices);
tuple<vector<Pixel>, vector<Pixel>> ComputePolygonRows(const vector<Pixel>& vertexPixels);


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
	vector<Triangle> triangles = LoadTestModel();
	Window window = Window::Create("Lab3", SCREEN_WIDTH, SCREEN_HEIGHT);
	Clock  clock  = Clock();

    Camera camera = { };
	camera.position = vec3(0.0, 0.0, 3.001);

	bool running = true;
	while (running)
	{
		float dt = clock.tick();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}

		Update(camera, dt);
		Draw(window, camera, triangles);

		// NOTE: The pixels are not shown on the screen 
		// until we update the window with this method.
		window.update();
	}

	window.screenshot();

	Window::Destroy(&window);
	return 0;
}

void Update(Camera& camera, float dt)
{
//	std::cout << "Render time: " << dt << " s." << std::endl;

	const Uint8* key_state = SDL_GetKeyboardState(nullptr);

	if (key_state[SDL_SCANCODE_W]) { camera.position.z -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_S]) { camera.position.z += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_D]) { camera.position.x += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_A]) { camera.position.x -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_UP])     { light_position.z -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_DOWN])   { light_position.z += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_RIGHT])  { light_position.x += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_LEFT])   { light_position.x -= 1.0f * dt; }
    if (key_state[SDL_SCANCODE_O])      { light_position.y += 1.0f * dt; }
    if (key_state[SDL_SCANCODE_L])      { light_position.y -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_LSHIFT]) { camera.position.y += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_SPACE])  { camera.position.y -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_E]) { camera.yaw += float(M_PI / 4.0) * dt; }
	if (key_state[SDL_SCANCODE_Q]) { camera.yaw -= float(M_PI / 4.0) * dt; }

//    int dx; int dy;
//    SDL_GetRelativeMouseState(&dx, &dy);
//    camera.yaw   -= float(dx) * dt * 0.1f;
//    camera.pitch += float(dy) * dt * 0.1f;
    camera.transform = rotation(camera.pitch, camera.yaw, camera.roll);
}


void Draw(Window& window, const Camera& camera, const vector<Triangle>& triangles)
{
	window.fill(BLACK);

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            depth_buffer[y][x] = 0;

	for (const auto& triangle : triangles)
	{
		vector<Pixel> polygon = {
		        VertexShader(camera, Vertex { triangle.v0, triangle.normal, vec2(0) }),
		        VertexShader(camera, Vertex { triangle.v1, triangle.normal, vec2(0) }),
		        VertexShader(camera, Vertex { triangle.v2, triangle.normal, vec2(0) }),
		};

        vector<Pixel> pixels = Rasterize(polygon);

        for (const Pixel& pixel : pixels)
            PixelShader(window, pixel, triangle);
	}
}


/// Expects t to be a value between 0-1.
float Interpolate(float a, float b, float t) {
    return (1.0f - t) * a  +  t * b;
}

/// High precision loss of `a` and `b` differs much in exponents.
float FastLerp(float a, float b, float t)
{
    return a + t * (b - a);
}

//
float FastLerp2(float a, float b, float t)
{
    return b - (b - a) * (1.0f - t);
}


vector<Pixel> Interpolate(Pixel a, Pixel b)
{
    vec3 start(a.x, a.y, a.z_inv);
    vec3 stop(b.x, b.y, b.z_inv);

    auto delta = glm::abs(stop - start);
    int pixels = int(glm::max(delta.x, delta.y)) + 1;
    vector<Pixel> line(pixels);

	auto step_a = vec3(stop - start) / float(glm::max(pixels - 1, 1));
    auto step_b = vec3(b.illumination - a.illumination) / float(glm::max(pixels - 1, 1));
	vec3 current_a(start);
    vec3 current_b(a.illumination);
	for (int i = 0; i < pixels; ++i)
	{
        line[i] = Pixel { int(current_a.x), int(current_a.y), current_a.z, current_b };
		current_a += step_a;
        current_b += step_b;
	}

    return line;
}

Pixel VertexShader(const Camera& camera, const Vertex& v)
{
    float f = -SCREEN_HEIGHT;
    vec3 p = camera.transform * (v.position - camera.position);
    auto x = int(f * p.x / p.z + (SCREEN_WIDTH  - 1) / 2.0f);
    auto y = int(f * p.y / p.z + (SCREEN_HEIGHT - 1) / 2.0f);

    // Reflectance
    const vec3  vertex_to_light    = light_position - v.position;
    const vec3  direction_to_light = normalize(vertex_to_light);
    const float radius = length(vertex_to_light);

    const float factor = max(dot(direction_to_light, v.normal), 0.0f);

    const vec3 specular = (factor * light_power) / (4.0f * float(M_PI) * radius * radius);
    const vec3 illumination = specular + indirect_light_power_per_area;

    auto result = glm::clamp(ivec2(x, y), ivec2(0, 0), ivec2(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));
    return { result.x, result.y, -1.0f / p.z, illumination };
}

vector<Pixel> Rasterize(const vector<Pixel>& polygon)
{
    auto[left, right] = ComputePolygonRows(polygon);
    vector<Pixel> pixels;
    pixels.reserve(left.size());
    for (int row = 0; row < left.size(); ++row)
        for (const Pixel& pixel : Interpolate(left[row], right[row]))
            pixels.push_back(pixel);

    return pixels;
}

void PixelShader(Window& window, const Pixel& pixel, const Triangle& triangle) {
    if (depth_buffer[pixel.y][pixel.x] < pixel.z_inv) {
        depth_buffer[pixel.y][pixel.x] = pixel.z_inv;
        window.set_pixel(pixel.x, pixel.y, clamp(triangle.color * pixel.illumination, vec3(0), vec3(1)));
    }
}




tuple<vector<Pixel>, vector<Pixel>> ComputePolygonRows(const vector<Pixel>& vertexPixels)
{
    const int vertexCount = vertexPixels.size();

	vector<float> yvalues(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
		yvalues[i] = vertexPixels[i].y;


	// Calculates max and min.
	auto max = std::max_element(yvalues.begin(), yvalues.end());
	auto min = std::min_element(yvalues.begin(), yvalues.end());

//	std::cout << "Y-values: " << *min << " - " << *max << std::endl;

	// Calculates number of rows according to the given formulas.
	auto numberOfRows = int(*max - *min) + 1;

//	std::cout << numberOfRows << std::endl;

    vector<Pixel> left(numberOfRows);
    vector<Pixel> right(numberOfRows);

	for (int i = 0; i < numberOfRows; ++i)
	{
        left[i].x  = std::numeric_limits<int>::max();
        right[i].x = std::numeric_limits<int>::min();
	}

	for (int i = 0; i < vertexCount; ++i)
	{
        int j = (i + 1) % vertexCount;

		auto v0 = vertexPixels[i];
		auto v1 = vertexPixels[j];

		auto line = Interpolate(v0, v1);

		for (const auto v : line) {

		    int row = int(v.y - *min + 0.5f);

		    auto& a = left[row];
            auto& b = right[row];

            if (a.x > v.x) { a = v; }
            if (b.x < v.x) { b = v; }
		}
	}

	return { left, right };
}
