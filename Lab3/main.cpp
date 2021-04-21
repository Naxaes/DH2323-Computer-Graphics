#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "SDL_helper.h"
#include "TestModel.h"

#include "../libraries/glm-0.9.9.8/glm/glm.hpp"


using std::vector;

using glm::vec3;
using glm::mat3;
using glm::ivec2;


// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH  = 500;
const int SCREEN_HEIGHT = 500;
vec3 cameraPos = vec3(0, 0, -3.001);



// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw(Window& window, const vector<Triangle>& triangles);
void Update(float dt);
void VertexShader(const vec3& v, ivec2& p);
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result);

void DrawLineSDL(Window& window, ivec2 a, ivec2 b, vec3 color);

void DrawPolygonEdges(Window& window, const vector<vec3>& vertices);

//template <typename T>
//T lerp(const T& start, const T& stop, float t)
//{
//	// t is a value between 0 - 1
//	// t = 0 returns start
//	// t = 1 returns stop
//	// Otherwise, return in between
//	return start * (1.0f - t) + stop * t;
//}
//
//template <typename T>
//std::vector<T> Interpolate(const T& start, const T& stop, int samples)
//{
//	std::vector<T> result;
//	result.reserve(samples);
//
//	if (samples == 1) {
//		result.push_back(start);
//		return result;
//	}
//
//	auto dividend = float(samples);
//	for (int i = 0; i < samples; ++i) {
//		auto value = lerp(start, stop, float(i) / dividend);
//		result.push_back(value);
//	}
//
//	return result;
//}
//

int main(int argc, char* argv[])
{
	vector<Triangle> triangles = LoadTestModel();
	Window window = Window::Create("Lab3", SCREEN_WIDTH, SCREEN_HEIGHT);
	Clock  clock  = Clock();

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
		Update(dt);
		Draw(window, triangles);

		// NOTE: The pixels are not shown on the screen 
		// until we update the window with this method.
		window.update();
	}

	window.screenshot();

	Window::Destroy(&window);
	return 0;
}

void Update(float dt)
{
	std::cout << "Render time: " << dt << " s." << std::endl;

	const Uint8* keystate = SDL_GetKeyboardState(nullptr);

	if (keystate[SDL_SCANCODE_UP])     { std::cout << "Pressing UP"		<< std::endl; }
	if (keystate[SDL_SCANCODE_DOWN])   { std::cout << "Pressing DOWN"	<< std::endl; }
	if (keystate[SDL_SCANCODE_RIGHT])  { std::cout << "Pressing RIGHT"	<< std::endl; }
	if (keystate[SDL_SCANCODE_LEFT])   { std::cout << "Pressing LEFT"	<< std::endl; }
	if (keystate[SDL_SCANCODE_RSHIFT]) { std::cout << "Pressing RSHIFT" << std::endl; }
	if (keystate[SDL_SCANCODE_RCTRL])  { std::cout << "Pressing RCTRL"	<< std::endl; }
	if (keystate[SDL_SCANCODE_W])      { std::cout << "Pressing W"		<< std::endl; }
	if (keystate[SDL_SCANCODE_S])      { std::cout << "Pressing S"		<< std::endl; }
	if (keystate[SDL_SCANCODE_D])      { std::cout << "Pressing D"		<< std::endl; }
	if (keystate[SDL_SCANCODE_A])      { std::cout << "Pressing A"		<< std::endl; }
	if (keystate[SDL_SCANCODE_E])      { std::cout << "Pressing E"		<< std::endl; }
	if (keystate[SDL_SCANCODE_Q])      { std::cout << "Pressing Q"		<< std::endl; }
}

void Draw(Window& window, const vector<Triangle>& triangles)
{
	window.fill(BLACK);

	for (const auto& triangle : triangles)
	{
		vector<vec3> vertices(3);

		vertices[0] = triangle.v0;
		vertices[1] = triangle.v1;
		vertices[2] = triangle.v2;

		// Add drawing

		DrawPolygonEdges(window, vertices);


		/*for (int v = 0; v < 3; ++v)
		{
			ivec2 projPos;
			for (int i = 0; i < 3; ++i)
				std::cout << projPos[i];
			VertexShader(vertices[v], projPos);
			vec3 color(1, 1, 1);
			//std::cout << projPos.x << std::endl;
			window.set_pixel(projPos.x, projPos.y, color);

		}
		vector<ivec2> lista(1000);

		ivec2 start = ivec2(2, 100);
		ivec2 end = ivec2(100, 100);

		DrawLineSDL(window, start, end, vec3(1)); */
	}
}
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result)
{
	int N = result.size();
	glm::vec2 step = glm::vec2(b - a) / float(glm::max(N - 1, 1));
	glm::vec2 current(a);
	for (int i = 0; i < N; ++i)
	{
		result[i] = current;
		current += step;
	}

}

void VertexShader(const vec3& v, ivec2& p)
{
	vec3 P = v - cameraPos;
	float f = SCREEN_HEIGHT; //Given in the instructions
	auto x = int(f * P.x / P.z + (SCREEN_WIDTH - 1) / 2.0f);
	auto y = int(f * P.y / P.z + (SCREEN_HEIGHT - 1) / 2.0f);
	p = glm::clamp(ivec2(x, y), ivec2(0,0), ivec2(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));

}


void DrawLineSDL(Window& window, ivec2 a, ivec2 b, vec3 color) {


	ivec2 delta = glm::abs(b - a);
	int pixels = glm::max(delta.x, delta.y) + 1;
	vector<ivec2> line(pixels);
	
	/*
	Interpolate(a, b, line);
	for (int i = 0; i < line.size(); ++i)
	{
		//std::cout << line[i].x << std::endl << line[i].y << std::endl;
		ivec2 nya = glm::clamp(ivec2(line[i].x, line[i].y), ivec2(0, 0), ivec2(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));
		window.set_pixel(nya.x, nya.y, vec3(1));
	}*/


	Interpolate(a, b, line);
	for (int v = 0; v < line.size(); ++v)
	{
		window.set_pixel(line[v].x, line[v].y, vec3(1));

	}

}

void DrawPolygonEdges(Window& window, const vector<vec3>& vertices)
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<ivec2> projectedVertices(V);
	for (int i = 0; i < V; ++i)
	{
		VertexShader(vertices[i], projectedVertices[i]);
	}
	// Loop over all vertices and draw the edge from it to the next vertex:
	for (int i = 0; i < V; ++i)
	{
		int j = (i + 1) % V; // The next vertex
		vec3 color(1, 1, 1);
		DrawLineSDL(window, projectedVertices[i], projectedVertices[j],color);
	}
}
