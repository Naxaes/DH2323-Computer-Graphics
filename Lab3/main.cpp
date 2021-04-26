#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "SDL_helper.h"
#include "TestModel.h"
#include <algorithm>

using std::vector;

using glm::vec3;
using glm::mat3;
using glm::ivec2;


// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
struct Camera {
	vec3  position;

	vec3  right;
	vec3  up;
	vec3  forward;

	float focal_length;
	float yaw;
	float pitch;
	float roll;
};

Camera camera;



const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
mat3 R;


// --------------------------------------------------------
// FUNCTION DECLARATIONS

void Draw(Window& window, const vector<Triangle>& triangles);
void Update(float dt);
void VertexShader(const vec3& v, ivec2& p);
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result);
void DrawLineSDL(Window& window, ivec2 a, ivec2 b, vec3 color);
void DrawPolygonEdges(Window& window, const vector<vec3>& vertices);
void ComputePolygonRows(const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels, vector<ivec2>& rightPixels);

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
	Clock  clock = Clock();

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

		int dx; int dy;
		SDL_GetRelativeMouseState(&dx, &dy);
		camera.yaw -= float(dx) * dt;
		camera.pitch += float(dy) * dt;
		R = rotation(camera.pitch, camera.yaw, camera.roll);
		//        camera.right   = vec3(R[0][0], R[0][1], R[0][2]);
		//        camera.up      = vec3(R[1][0], R[1][1], R[1][2]);
		//        camera.forward = vec3(R[2][0], R[2][1], R[2][2]);

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
	//std::cout << "Render time: " << dt << " s." << std::endl;

	const Uint8* key_state = SDL_GetKeyboardState(nullptr);

	if (key_state[SDL_SCANCODE_W]) { camera.position.z -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_S]) { camera.position.z += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_D]) { camera.position.x += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_A]) { camera.position.x -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_LSHIFT]) { camera.position.y += 1.0f * dt; }
	if (key_state[SDL_SCANCODE_SPACE]) { camera.position.y -= 1.0f * dt; }
	if (key_state[SDL_SCANCODE_E]) { camera.yaw += float(M_PI / 4.0) * dt; }
	if (key_state[SDL_SCANCODE_Q]) { camera.yaw -= float(M_PI / 4.0) * dt; }
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
		vector<ivec2> vertexPixels(3);
		vertexPixels[0] = ivec2(10, 5);
		vertexPixels[1] = ivec2(5, 10);
		vertexPixels[2] = ivec2(15, 15);
		vector<ivec2> leftPixels;
		vector<ivec2> rightPixels;
		ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
		for (int row = 0; row < leftPixels.size(); ++row)
		{
			std::cout << "Start: ("
				<< leftPixels[row].x << ","
				<< leftPixels[row].y << "). "
				<< "End: ("
				<< rightPixels[row].x << ","
				<< rightPixels[row].y << "). " << std::endl;
		}

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
	float f = -SCREEN_HEIGHT; //Given in the instructions
	vec3 P = R * (v - camera.position);
	auto x = int(f * P.x / P.z + (SCREEN_WIDTH - 1) / 2.0f);
	auto y = int(f * P.y / P.z + (SCREEN_HEIGHT - 1) / 2.0f);
	p = glm::clamp(ivec2(x, y), ivec2(0, 0), ivec2(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));
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
		DrawLineSDL(window, projectedVertices[i], projectedVertices[j], color);
	}
}
void ComputePolygonRows(const vector<ivec2>& vertexPixels, vector<ivec2>& leftPixels, vector<ivec2>& rightPixels)
{
	vector<float> yvalues;
	int numberOfrows;
	int max;
	int min;
	for (int i = 0; i < vertexPixels.size(); ++i)
	{
		yvalues.push_back(vertexPixels[i].y);
	}

	
	//Caluclates max and min
	max = *std::max_element(yvalues.begin(), yvalues.end());
	min = *std::min_element(yvalues.begin(), yvalues.end());

	//std::cout << yvalues[i] << std::endl;

	//Caluclates number of rows according to the given formulas
	numberOfrows = max - min + 1;

	std::cout << numberOfrows << std::endl;
	leftPixels.resize(numberOfrows);
	rightPixels.resize(numberOfrows);

	for (int i = 0; i < numberOfrows; ++i)
	{
		leftPixels[i].x = +std::numeric_limits<int>::max();
		rightPixels[i].x = -std::numeric_limits<int>::max();
	}

	vector<ivec2> number(numberOfrows);


	for (int i = 0; i < vertexPixels.size(); ++i)
	{
		ivec2 firstedge = vertexPixels[i];
		ivec2 end = vertexPixels[i+1];
		Interpolate(firstedge, end, number);
		std::cout << number[i].x;

	}
	
	//std::max_element(vertexPixels);
	// 1. Find max and min y-value of the polygon
	// and compute the number of rows it occupies.
	// 2. Resize leftPixels and rightPixels
	// so that they have an element for each row.
	// 3. Initialize the x-coordinates in leftPixels
	// to some really large value and the x-coordinates
	// in rightPixels to some really small value.
	// 4. Loop through all edges of the polygon and use
	// linear interpolation to find the x-coordinate for
	// each row it occupies. Update the corresponding
	// values in rightPixels and leftPixels.


}
