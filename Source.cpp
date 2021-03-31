#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

vector<Triangle> triangles;

// ----------------------------------------------------------------------------
// STRUCTS

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

Intersection closestIntersection;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);

int main(int argc, char* argv[])
{
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.



	while (NoQuitMessageSDL())
	{
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			vec3 color(1, 0.5, 0.5);
			PutPixelSDL(screen, x, y, color);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	float m = std::numeric_limits<float>::max();
	float tray, u, v;
	vec3 s = start;
	vec3 d = dir;

	for (int i = 0; i < triangles.size(); ++i) {
		Triangle triangle = triangles[i];
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = s - v0;
		mat3 A(-d, e1, e2);
		vec3 x = glm::inverse(A) * b;

		//We know from the instructions that x = (t, u, v) where 0 <= t, 0 < u, 0 < v, u + v < 1
		//We name t tray instead since we already have a global variable t for time
		//To get the intersection we create an if-statement

		tray = x[0];
		u = x[1];
		v = x[2];

		if (tray >= 0) {
			if (u > 0) {
				if (v > 0) {
					if (u + v < 1) {
						vec3 pos = v0 + u * e1 + v * e2; //Could use other ways to calculate??
						float dis = glm::length(pos - s); //Could use other ways to calculate??
						if (dis < m) {
							m = dis;
							closestIntersection.position = pos;
							closestIntersection.distance = dis;
							closestIntersection.triangleIndex = i;
						}
					}
				}
			}
		}
	}
	

	if (m < std::numeric_limits<float>::max()) {
		return true;
	}
	else {
		return false;
	}
}
