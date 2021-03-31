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

const int SCREEN_WIDTH  = 100;
const int SCREEN_HEIGHT = 100;
SDL_Surface* screen;
int t;

vector<Triangle> triangles;

float yaw = 0.0f;

// ----------------------------------------------------------------------------
// STRUCTS

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};


// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw(vec3 cameraPos);
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);

int main(int argc, char* argv[])
{
    triangles = LoadTestModel();

	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.

    Uint8* keystate = SDL_GetKeyState(nullptr);

    /*
    mat3 rotation = mat3{
        vec3{ cos(yaw), -sin(yaw), 0  },
        vec3{ sin(yaw),  cos(yaw), 0  },
        vec3{    0,         0,     1  },
    };

    auto R = rotation;
    vec3 right   ( R[0][0], R[0][1], R[0][2] );
    vec3 down    ( R[1][0], R[1][1], R[1][2] );
    vec3 forward ( R[2][0], R[2][1], R[2][2] );
    */

    vec3 cameraPos(0.0f, 0.0f, 2.0f);
	while (NoQuitMessageSDL())
	{
        if (keystate[SDLK_UP])
        {
            // Move camera forward
            cameraPos.z -= 0.1f;
        }
        if (keystate[SDLK_DOWN])
        {
            // Move camera backward
            cameraPos.z += 0.1f;
        }
        if (keystate[SDLK_LEFT])
        {
            // Move camera to the left
            cameraPos.x -= 0.1f;
        }
        if (keystate[SDLK_RIGHT])
        {
            // Move camera to the right
            cameraPos.x += 0.1f;
        }

		Update();
		Draw(cameraPos);
	}

	SDL_SaveBMP(screen, "../../Lab2/screenshot" __TIME__ ".bmp");
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

void Draw(vec3 cameraPos)
{
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

    auto W = float(SCREEN_WIDTH);
    auto H = float(SCREEN_HEIGHT);

    float F = H / 2.0f;
    Intersection closestIntersection;

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
            auto direction = glm::normalize(vec3(x - W/2.0, y - H/2.0, -F));
            if (ClosestIntersection(cameraPos, direction, triangles, closestIntersection))
            {
                auto triangle = triangles[closestIntersection.triangleIndex];
                PutPixelSDL(screen, x, y, triangle.color);
            }
            else
            {
                PutPixelSDL(screen, x, y, vec3(0.0f, 0.0f, 0.0f));
            }
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
			if (u >= 0) {
				if (v >= 0) {
					if (u + v <= 1) {
					    vec3 pos = start + dir * tray;
						// vec3 pos = v0 + u * e1 + v * e2; //Could use other ways to calculate??
						float dis = tray; // glm::length(pos - s); //Could use other ways to calculate??
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
