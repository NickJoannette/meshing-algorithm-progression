#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include "OpenGLWindow.h"
#include "Shader.h"
#include "UI_InputManager.h"
#include "Camera.h"
#include "EditablePlane.h"
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_glfw.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <Noise/FastNoise.h>
#include "PhysicsCommon.h"
#include "Common.h"
#include <Meshes/Cube.h>
#include <Meshes/Sphere.h>
#include <Meshes/Line.h>
#include <Meshes/FreeMesh.h>
#include <MeshingAlgorithms/MarchingCubesStanford.h>
#include <MeshingAlgorithms/MarchingCubes.h>
#include "PhysicsRealm.h";

using namespace glm;

// Physics definitionsw
float FORCE_OF_GRAVITY = 9.81f;
float GRAVITATIONAL_ACCELERATION = 9.81f;
float AIR_DRAG = 0.01f;
float MAX_PLAYER_SPEED = 5.0f;
float MAX_PLAYER_JUMP_ACCELERATION = 9.81f;
float TIME_STEP_FACTOR = 50.0f;
float BUILD_STRENGTH = .50f;
float PerlinThreshold1 = 0.0f;
float MarchingCubeCornerThreshold = 0.0f;
bool ApplyRLE = true;
int Seed = 314142123;

static const int chunkWidth = 12;
static const int chunkHeight = 12;
static const int chunkLength = 12;




GRIDCELL WORLD_GRID[chunkWidth][chunkHeight][chunkLength];
CELL_CORNER_PAIR VERTEX_NEIGHBORS[chunkWidth][chunkHeight][chunkLength][8];

Voxel chunk[chunkWidth][chunkHeight][chunkLength];

// Constants for defining window sizew
const float SCR_WIDTH = 1920.0, SCR_HEIGHT = 1080.0;

OpenGLWindow * mainWindow = new OpenGLWindow(SCR_WIDTH, SCR_HEIGHT);
GLFWwindow * mWind = mainWindow->glWindow();
Camera camera(mainWindow, glm::vec3(0, 3, 25));
Shader gridShader("Shaders/gridShader.vs", "Shaders/gridShader.fs");
Shader blockShader("Shaders/blockShader.vs", "Shaders/blockShader.fs");

EditablePlane ep(50, 50);
UI_InputManager IM(mWind, &camera);

static const int edgeTable[256] = {
  0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
  0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
  0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
  0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
  0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
  0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
  0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
  0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
  0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
  0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
  0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
  0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
  0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
  0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
  0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
  0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
  0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
  0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
  0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
  0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
  0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
  0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
  0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
  0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
  0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
  0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
  0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
  0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
  0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
  0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
  0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
  0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

static const int triTable[256][16] = {
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  8,  3,  9,  8,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  2, 10,  0,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 2,  8,  3,  2, 10,  8, 10,  9,  8, -1, -1, -1, -1, -1, -1, -1},
  { 3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0, 11,  2,  8, 11,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  9,  0,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1, 11,  2,  1,  9, 11,  9,  8, 11, -1, -1, -1, -1, -1, -1, -1},
  { 3, 10,  1, 11, 10,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0, 10,  1,  0,  8, 10,  8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
  { 3,  9,  0,  3, 11,  9, 11, 10,  9, -1, -1, -1, -1, -1, -1, -1},
  { 9,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  3,  0,  7,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  1,  9,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  1,  9,  4,  7,  1,  7,  3,  1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  4,  7,  3,  0,  4,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1},
  { 9,  2, 10,  9,  0,  2,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1},
  { 2, 10,  9,  2,  9,  7,  2,  7,  3,  7,  9,  4, -1, -1, -1, -1},
  { 8,  4,  7,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11,  4,  7, 11,  2,  4,  2,  0,  4, -1, -1, -1, -1, -1, -1, -1},
  { 9,  0,  1,  8,  4,  7,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1},
  { 4,  7, 11,  9,  4, 11,  9, 11,  2,  9,  2,  1, -1, -1, -1, -1},
  { 3, 10,  1,  3, 11, 10,  7,  8,  4, -1, -1, -1, -1, -1, -1, -1},
  { 1, 11, 10,  1,  4, 11,  1,  0,  4,  7, 11,  4, -1, -1, -1, -1},
  { 4,  7,  8,  9,  0, 11,  9, 11, 10, 11,  0,  3, -1, -1, -1, -1},
  { 4,  7, 11,  4, 11,  9,  9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
  { 9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  5,  4,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  5,  4,  1,  5,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 8,  5,  4,  8,  3,  5,  3,  1,  5, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  0,  8,  1,  2, 10,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1},
  { 5,  2, 10,  5,  4,  2,  4,  0,  2, -1, -1, -1, -1, -1, -1, -1},
  { 2, 10,  5,  3,  2,  5,  3,  5,  4,  3,  4,  8, -1, -1, -1, -1},
  { 9,  5,  4,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0, 11,  2,  0,  8, 11,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1},
  { 0,  5,  4,  0,  1,  5,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1},
  { 2,  1,  5,  2,  5,  8,  2,  8, 11,  4,  8,  5, -1, -1, -1, -1},
  {10,  3, 11, 10,  1,  3,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1},
  { 4,  9,  5,  0,  8,  1,  8, 10,  1,  8, 11, 10, -1, -1, -1, -1},
  { 5,  4,  0,  5,  0, 11,  5, 11, 10, 11,  0,  3, -1, -1, -1, -1},
  { 5,  4,  8,  5,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1},
  { 9,  7,  8,  5,  7,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  3,  0,  9,  5,  3,  5,  7,  3, -1, -1, -1, -1, -1, -1, -1},
  { 0,  7,  8,  0,  1,  7,  1,  5,  7, -1, -1, -1, -1, -1, -1, -1},
  { 1,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  7,  8,  9,  5,  7, 10,  1,  2, -1, -1, -1, -1, -1, -1, -1},
  {10,  1,  2,  9,  5,  0,  5,  3,  0,  5,  7,  3, -1, -1, -1, -1},
  { 8,  0,  2,  8,  2,  5,  8,  5,  7, 10,  5,  2, -1, -1, -1, -1},
  { 2, 10,  5,  2,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1},
  { 7,  9,  5,  7,  8,  9,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1},
  { 9,  5,  7,  9,  7,  2,  9,  2,  0,  2,  7, 11, -1, -1, -1, -1},
  { 2,  3, 11,  0,  1,  8,  1,  7,  8,  1,  5,  7, -1, -1, -1, -1},
  {11,  2,  1, 11,  1,  7,  7,  1,  5, -1, -1, -1, -1, -1, -1, -1},
  { 9,  5,  8,  8,  5,  7, 10,  1,  3, 10,  3, 11, -1, -1, -1, -1},
  { 5,  7,  0,  5,  0,  9,  7, 11,  0,  1,  0, 10, 11, 10,  0, -1},
  {11, 10,  0, 11,  0,  3, 10,  5,  0,  8,  0,  7,  5,  7,  0, -1},
  {11, 10,  5,  7, 11,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  0,  1,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  8,  3,  1,  9,  8,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1},
  { 1,  6,  5,  2,  6,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  6,  5,  1,  2,  6,  3,  0,  8, -1, -1, -1, -1, -1, -1, -1},
  { 9,  6,  5,  9,  0,  6,  0,  2,  6, -1, -1, -1, -1, -1, -1, -1},
  { 5,  9,  8,  5,  8,  2,  5,  2,  6,  3,  2,  8, -1, -1, -1, -1},
  { 2,  3, 11, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11,  0,  8, 11,  2,  0, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1},
  { 0,  1,  9,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1},
  { 5, 10,  6,  1,  9,  2,  9, 11,  2,  9,  8, 11, -1, -1, -1, -1},
  { 6,  3, 11,  6,  5,  3,  5,  1,  3, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8, 11,  0, 11,  5,  0,  5,  1,  5, 11,  6, -1, -1, -1, -1},
  { 3, 11,  6,  0,  3,  6,  0,  6,  5,  0,  5,  9, -1, -1, -1, -1},
  { 6,  5,  9,  6,  9, 11, 11,  9,  8, -1, -1, -1, -1, -1, -1, -1},
  { 5, 10,  6,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  3,  0,  4,  7,  3,  6,  5, 10, -1, -1, -1, -1, -1, -1, -1},
  { 1,  9,  0,  5, 10,  6,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1},
  {10,  6,  5,  1,  9,  7,  1,  7,  3,  7,  9,  4, -1, -1, -1, -1},
  { 6,  1,  2,  6,  5,  1,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2,  5,  5,  2,  6,  3,  0,  4,  3,  4,  7, -1, -1, -1, -1},
  { 8,  4,  7,  9,  0,  5,  0,  6,  5,  0,  2,  6, -1, -1, -1, -1},
  { 7,  3,  9,  7,  9,  4,  3,  2,  9,  5,  9,  6,  2,  6,  9, -1},
  { 3, 11,  2,  7,  8,  4, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1},
  { 5, 10,  6,  4,  7,  2,  4,  2,  0,  2,  7, 11, -1, -1, -1, -1},
  { 0,  1,  9,  4,  7,  8,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1},
  { 9,  2,  1,  9, 11,  2,  9,  4, 11,  7, 11,  4,  5, 10,  6, -1},
  { 8,  4,  7,  3, 11,  5,  3,  5,  1,  5, 11,  6, -1, -1, -1, -1},
  { 5,  1, 11,  5, 11,  6,  1,  0, 11,  7, 11,  4,  0,  4, 11, -1},
  { 0,  5,  9,  0,  6,  5,  0,  3,  6, 11,  6,  3,  8,  4,  7, -1},
  { 6,  5,  9,  6,  9, 11,  4,  7,  9,  7, 11,  9, -1, -1, -1, -1},
  {10,  4,  9,  6,  4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4, 10,  6,  4,  9, 10,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1},
  {10,  0,  1, 10,  6,  0,  6,  4,  0, -1, -1, -1, -1, -1, -1, -1},
  { 8,  3,  1,  8,  1,  6,  8,  6,  4,  6,  1, 10, -1, -1, -1, -1},
  { 1,  4,  9,  1,  2,  4,  2,  6,  4, -1, -1, -1, -1, -1, -1, -1},
  { 3,  0,  8,  1,  2,  9,  2,  4,  9,  2,  6,  4, -1, -1, -1, -1},
  { 0,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 8,  3,  2,  8,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1},
  {10,  4,  9, 10,  6,  4, 11,  2,  3, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  2,  2,  8, 11,  4,  9, 10,  4, 10,  6, -1, -1, -1, -1},
  { 3, 11,  2,  0,  1,  6,  0,  6,  4,  6,  1, 10, -1, -1, -1, -1},
  { 6,  4,  1,  6,  1, 10,  4,  8,  1,  2,  1, 11,  8, 11,  1, -1},
  { 9,  6,  4,  9,  3,  6,  9,  1,  3, 11,  6,  3, -1, -1, -1, -1},
  { 8, 11,  1,  8,  1,  0, 11,  6,  1,  9,  1,  4,  6,  4,  1, -1},
  { 3, 11,  6,  3,  6,  0,  0,  6,  4, -1, -1, -1, -1, -1, -1, -1},
  { 6,  4,  8, 11,  6,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 7, 10,  6,  7,  8, 10,  8,  9, 10, -1, -1, -1, -1, -1, -1, -1},
  { 0,  7,  3,  0, 10,  7,  0,  9, 10,  6,  7, 10, -1, -1, -1, -1},
  {10,  6,  7,  1, 10,  7,  1,  7,  8,  1,  8,  0, -1, -1, -1, -1},
  {10,  6,  7, 10,  7,  1,  1,  7,  3, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2,  6,  1,  6,  8,  1,  8,  9,  8,  6,  7, -1, -1, -1, -1},
  { 2,  6,  9,  2,  9,  1,  6,  7,  9,  0,  9,  3,  7,  3,  9, -1},
  { 7,  8,  0,  7,  0,  6,  6,  0,  2, -1, -1, -1, -1, -1, -1, -1},
  { 7,  3,  2,  6,  7,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 2,  3, 11, 10,  6,  8, 10,  8,  9,  8,  6,  7, -1, -1, -1, -1},
  { 2,  0,  7,  2,  7, 11,  0,  9,  7,  6,  7, 10,  9, 10,  7, -1},
  { 1,  8,  0,  1,  7,  8,  1, 10,  7,  6,  7, 10,  2,  3, 11, -1},
  {11,  2,  1, 11,  1,  7, 10,  6,  1,  6,  7,  1, -1, -1, -1, -1},
  { 8,  9,  6,  8,  6,  7,  9,  1,  6, 11,  6,  3,  1,  3,  6, -1},
  { 0,  9,  1, 11,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 7,  8,  0,  7,  0,  6,  3, 11,  0, 11,  6,  0, -1, -1, -1, -1},
  { 7, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  0,  8, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  1,  9, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 8,  1,  9,  8,  3,  1, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1},
  {10,  1,  2,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10,  3,  0,  8,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1},
  { 2,  9,  0,  2, 10,  9,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1},
  { 6, 11,  7,  2, 10,  3, 10,  8,  3, 10,  9,  8, -1, -1, -1, -1},
  { 7,  2,  3,  6,  2,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 7,  0,  8,  7,  6,  0,  6,  2,  0, -1, -1, -1, -1, -1, -1, -1},
  { 2,  7,  6,  2,  3,  7,  0,  1,  9, -1, -1, -1, -1, -1, -1, -1},
  { 1,  6,  2,  1,  8,  6,  1,  9,  8,  8,  7,  6, -1, -1, -1, -1},
  {10,  7,  6, 10,  1,  7,  1,  3,  7, -1, -1, -1, -1, -1, -1, -1},
  {10,  7,  6,  1,  7, 10,  1,  8,  7,  1,  0,  8, -1, -1, -1, -1},
  { 0,  3,  7,  0,  7, 10,  0, 10,  9,  6, 10,  7, -1, -1, -1, -1},
  { 7,  6, 10,  7, 10,  8,  8, 10,  9, -1, -1, -1, -1, -1, -1, -1},
  { 6,  8,  4, 11,  8,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  6, 11,  3,  0,  6,  0,  4,  6, -1, -1, -1, -1, -1, -1, -1},
  { 8,  6, 11,  8,  4,  6,  9,  0,  1, -1, -1, -1, -1, -1, -1, -1},
  { 9,  4,  6,  9,  6,  3,  9,  3,  1, 11,  3,  6, -1, -1, -1, -1},
  { 6,  8,  4,  6, 11,  8,  2, 10,  1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10,  3,  0, 11,  0,  6, 11,  0,  4,  6, -1, -1, -1, -1},
  { 4, 11,  8,  4,  6, 11,  0,  2,  9,  2, 10,  9, -1, -1, -1, -1},
  {10,  9,  3, 10,  3,  2,  9,  4,  3, 11,  3,  6,  4,  6,  3, -1},
  { 8,  2,  3,  8,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1},
  { 0,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  9,  0,  2,  3,  4,  2,  4,  6,  4,  3,  8, -1, -1, -1, -1},
  { 1,  9,  4,  1,  4,  2,  2,  4,  6, -1, -1, -1, -1, -1, -1, -1},
  { 8,  1,  3,  8,  6,  1,  8,  4,  6,  6, 10,  1, -1, -1, -1, -1},
  {10,  1,  0, 10,  0,  6,  6,  0,  4, -1, -1, -1, -1, -1, -1, -1},
  { 4,  6,  3,  4,  3,  8,  6, 10,  3,  0,  3,  9, 10,  9,  3, -1},
  {10,  9,  4,  6, 10,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  9,  5,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3,  4,  9,  5, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1},
  { 5,  0,  1,  5,  4,  0,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1},
  {11,  7,  6,  8,  3,  4,  3,  5,  4,  3,  1,  5, -1, -1, -1, -1},
  { 9,  5,  4, 10,  1,  2,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1},
  { 6, 11,  7,  1,  2, 10,  0,  8,  3,  4,  9,  5, -1, -1, -1, -1},
  { 7,  6, 11,  5,  4, 10,  4,  2, 10,  4,  0,  2, -1, -1, -1, -1},
  { 3,  4,  8,  3,  5,  4,  3,  2,  5, 10,  5,  2, 11,  7,  6, -1},
  { 7,  2,  3,  7,  6,  2,  5,  4,  9, -1, -1, -1, -1, -1, -1, -1},
  { 9,  5,  4,  0,  8,  6,  0,  6,  2,  6,  8,  7, -1, -1, -1, -1},
  { 3,  6,  2,  3,  7,  6,  1,  5,  0,  5,  4,  0, -1, -1, -1, -1},
  { 6,  2,  8,  6,  8,  7,  2,  1,  8,  4,  8,  5,  1,  5,  8, -1},
  { 9,  5,  4, 10,  1,  6,  1,  7,  6,  1,  3,  7, -1, -1, -1, -1},
  { 1,  6, 10,  1,  7,  6,  1,  0,  7,  8,  7,  0,  9,  5,  4, -1},
  { 4,  0, 10,  4, 10,  5,  0,  3, 10,  6, 10,  7,  3,  7, 10, -1},
  { 7,  6, 10,  7, 10,  8,  5,  4, 10,  4,  8, 10, -1, -1, -1, -1},
  { 6,  9,  5,  6, 11,  9, 11,  8,  9, -1, -1, -1, -1, -1, -1, -1},
  { 3,  6, 11,  0,  6,  3,  0,  5,  6,  0,  9,  5, -1, -1, -1, -1},
  { 0, 11,  8,  0,  5, 11,  0,  1,  5,  5,  6, 11, -1, -1, -1, -1},
  { 6, 11,  3,  6,  3,  5,  5,  3,  1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 10,  9,  5, 11,  9, 11,  8, 11,  5,  6, -1, -1, -1, -1},
  { 0, 11,  3,  0,  6, 11,  0,  9,  6,  5,  6,  9,  1,  2, 10, -1},
  {11,  8,  5, 11,  5,  6,  8,  0,  5, 10,  5,  2,  0,  2,  5, -1},
  { 6, 11,  3,  6,  3,  5,  2, 10,  3, 10,  5,  3, -1, -1, -1, -1},
  { 5,  8,  9,  5,  2,  8,  5,  6,  2,  3,  8,  2, -1, -1, -1, -1},
  { 9,  5,  6,  9,  6,  0,  0,  6,  2, -1, -1, -1, -1, -1, -1, -1},
  { 1,  5,  8,  1,  8,  0,  5,  6,  8,  3,  8,  2,  6,  2,  8, -1},
  { 1,  5,  6,  2,  1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  3,  6,  1,  6, 10,  3,  8,  6,  5,  6,  9,  8,  9,  6, -1},
  {10,  1,  0, 10,  0,  6,  9,  5,  0,  5,  6,  0, -1, -1, -1, -1},
  { 0,  3,  8,  5,  6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11,  5, 10,  7,  5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11,  5, 10, 11,  7,  5,  8,  3,  0, -1, -1, -1, -1, -1, -1, -1},
  { 5, 11,  7,  5, 10, 11,  1,  9,  0, -1, -1, -1, -1, -1, -1, -1},
  {10,  7,  5, 10, 11,  7,  9,  8,  1,  8,  3,  1, -1, -1, -1, -1},
  {11,  1,  2, 11,  7,  1,  7,  5,  1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3,  1,  2,  7,  1,  7,  5,  7,  2, 11, -1, -1, -1, -1},
  { 9,  7,  5,  9,  2,  7,  9,  0,  2,  2, 11,  7, -1, -1, -1, -1},
  { 7,  5,  2,  7,  2, 11,  5,  9,  2,  3,  2,  8,  9,  8,  2, -1},
  { 2,  5, 10,  2,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1},
  { 8,  2,  0,  8,  5,  2,  8,  7,  5, 10,  2,  5, -1, -1, -1, -1},
  { 9,  0,  1,  5, 10,  3,  5,  3,  7,  3, 10,  2, -1, -1, -1, -1},
  { 9,  8,  2,  9,  2,  1,  8,  7,  2, 10,  2,  5,  7,  5,  2, -1},
  { 1,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  7,  0,  7,  1,  1,  7,  5, -1, -1, -1, -1, -1, -1, -1},
  { 9,  0,  3,  9,  3,  5,  5,  3,  7, -1, -1, -1, -1, -1, -1, -1},
  { 9,  8,  7,  5,  9,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 5,  8,  4,  5, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1},
  { 5,  0,  4,  5, 11,  0,  5, 10, 11, 11,  3,  0, -1, -1, -1, -1},
  { 0,  1,  9,  8,  4, 10,  8, 10, 11, 10,  4,  5, -1, -1, -1, -1},
  {10, 11,  4, 10,  4,  5, 11,  3,  4,  9,  4,  1,  3,  1,  4, -1},
  { 2,  5,  1,  2,  8,  5,  2, 11,  8,  4,  5,  8, -1, -1, -1, -1},
  { 0,  4, 11,  0, 11,  3,  4,  5, 11,  2, 11,  1,  5,  1, 11, -1},
  { 0,  2,  5,  0,  5,  9,  2, 11,  5,  4,  5,  8, 11,  8,  5, -1},
  { 9,  4,  5,  2, 11,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 2,  5, 10,  3,  5,  2,  3,  4,  5,  3,  8,  4, -1, -1, -1, -1},
  { 5, 10,  2,  5,  2,  4,  4,  2,  0, -1, -1, -1, -1, -1, -1, -1},
  { 3, 10,  2,  3,  5, 10,  3,  8,  5,  4,  5,  8,  0,  1,  9, -1},
  { 5, 10,  2,  5,  2,  4,  1,  9,  2,  9,  4,  2, -1, -1, -1, -1},
  { 8,  4,  5,  8,  5,  3,  3,  5,  1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  4,  5,  1,  0,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 8,  4,  5,  8,  5,  3,  9,  0,  5,  0,  3,  5, -1, -1, -1, -1},
  { 9,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4, 11,  7,  4,  9, 11,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
  { 0,  8,  3,  4,  9,  7,  9, 11,  7,  9, 10, 11, -1, -1, -1, -1},
  { 1, 10, 11,  1, 11,  4,  1,  4,  0,  7,  4, 11, -1, -1, -1, -1},
  { 3,  1,  4,  3,  4,  8,  1, 10,  4,  7,  4, 11, 10, 11,  4, -1},
  { 4, 11,  7,  9, 11,  4,  9,  2, 11,  9,  1,  2, -1, -1, -1, -1},
  { 9,  7,  4,  9, 11,  7,  9,  1, 11,  2, 11,  1,  0,  8,  3, -1},
  {11,  7,  4, 11,  4,  2,  2,  4,  0, -1, -1, -1, -1, -1, -1, -1},
  {11,  7,  4, 11,  4,  2,  8,  3,  4,  3,  2,  4, -1, -1, -1, -1},
  { 2,  9, 10,  2,  7,  9,  2,  3,  7,  7,  4,  9, -1, -1, -1, -1},
  { 9, 10,  7,  9,  7,  4, 10,  2,  7,  8,  7,  0,  2,  0,  7, -1},
  { 3,  7, 10,  3, 10,  2,  7,  4, 10,  1, 10,  0,  4,  0, 10, -1},
  { 1, 10,  2,  8,  7,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  9,  1,  4,  1,  7,  7,  1,  3, -1, -1, -1, -1, -1, -1, -1},
  { 4,  9,  1,  4,  1,  7,  0,  8,  1,  8,  7,  1, -1, -1, -1, -1},
  { 4,  0,  3,  7,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 4,  8,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 9, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  0,  9,  3,  9, 11, 11,  9, 10, -1, -1, -1, -1, -1, -1, -1},
  { 0,  1, 10,  0, 10,  8,  8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
  { 3,  1, 10, 11,  3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  2, 11,  1, 11,  9,  9, 11,  8, -1, -1, -1, -1, -1, -1, -1},
  { 3,  0,  9,  3,  9, 11,  1,  2,  9,  2, 11,  9, -1, -1, -1, -1},
  { 0,  2, 11,  8,  0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 3,  2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 2,  3,  8,  2,  8, 10, 10,  8,  9, -1, -1, -1, -1, -1, -1, -1},
  { 9, 10,  2,  0,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 2,  3,  8,  2,  8, 10,  0,  1,  8,  1, 10,  8, -1, -1, -1, -1},
  { 1, 10,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 1,  3,  8,  9,  1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  9,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  3,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};




void setupIMGUI() {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	const char* glsl_version = "#version 330";
	ImGui::CreateContext();
	ImGuiIO& mainMenu = ImGui::GetIO(); (void)mainMenu;
	mainMenu.IniFilename = NULL;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWind, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 lightAmbient = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 lightDiffuse = ImVec4(0.52f, 0.52f, 0.52f, 1.0f);
	ImVec4 lightSpecular = ImVec4(0.55f, 0.52f, 0.52f, 1.0f);

}

Cube cube;
void iterateOnChunkVoxels(void (*f) (vec3)) {

	int count = 0;
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 16; y++)
			for (int z = 0; z < 16; z++)
				if (!chunk[x][y][z].culled) {// If it's a visible/un-culled voxel
					(*f)(chunk[x][y][z].start);
					count++;
				}

	std::cout << "# Drawn: " << count << std::endl;

}

void drawCube(vec3 start) {
	blockShader.setMat4("transform", translate(start));
	cube.Draw(IM.primitiveType);
}

struct TriangularFace {
	vec3 v1, v2, v3;
};


const int EMPTY_SPACE = 0, STRUCTURAL = 1;

FreeMesh FirstAttemptedMesh;
void Phase1And2And3() {
	// Phase 1 and 2

	FastNoiseLite fastNoise((int)(Seed));
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	for (int x = 0; x < chunkWidth; x++)
		for (int y = 0; y < chunkHeight; y++)
			for (int z = 0; z < chunkLength; z++) {
				Voxel* voxel = &chunk[x][y][z];
				voxel->density = fastNoise.GetNoise((float)x*10, (float)y*10, (float)z* 10);
				voxel->start = vec3(x, y, z);
				voxel->culled = false;
				if (voxel->density >= PerlinThreshold1) voxel->type = STRUCTURAL;
				else {
					voxel->type = EMPTY_SPACE;
					voxel->culled = true;
				}
			}


	// Phase 3 - NAIVE. Just generating all faces for every cube.
	
	// Clear the existing vertices if necessary
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.vertices.clear();

	int NumTrianglesRendered = 0;
	float vSL = 1.0f; // Voxel Side Length
	for (int x = 0; x < chunkWidth; x++)
		for (int y = 0; y < chunkHeight; y++)
			for (int z = 0; z < chunkLength; z++) {

				// This Voxel
				Voxel * voxel = &chunk[x][y][z];

				if (voxel->type == EMPTY_SPACE) continue;

				vec3 vO = voxel->start; // Voxel Origin

				// Generate the vertices for the 2 triangles of the -X face
				// ... Only if there isn't already a structural voxel along that axis
				if (x == 0 || chunk[x-1][y][z].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x, vO.y, vO.z };
					Vertex v2{ vO.x, vO.y, vO.z + vSL };
					Vertex v3{ vO.x, vO.y + vSL, vO.z + vSL };

					Vertex v4{ vO.x, vO.y, vO.z };
					Vertex v5{ vO.x, vO.y + vSL, vO.z + vSL };
					Vertex v6{ vO.x, vO.y + vSL, vO.z };

					v1.nx = -1; v1.ny = 0; v1.nz = 0;
					v2.nx = -1; v2.ny = 0; v2.nz = 0;
					v3.nx = -1; v3.ny = 0; v3.nz = 0;
					v4.nx = -1; v4.ny = 0; v4.nz = 0;
					v5.nx = -1; v5.ny = 0; v5.nz = 0;
					v6.nx = -1; v6.ny = 0; v6.nz = 0;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;
				}
				// Generate the vertices for the 2 triangles of the +X face
				if (x == chunkWidth - 1 || chunk[x + 1][y][z].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x + vSL, vO.y, vO.z };
					Vertex v2{ vO.x + vSL, vO.y + vSL, vO.z };
					Vertex v3{ vO.x + vSL, vO.y + vSL, vO.z + vSL };

					Vertex v4{ vO.x + vSL, vO.y, vO.z + vSL };
					Vertex v5{ vO.x + vSL, vO.y, vO.z };
					Vertex v6{ vO.x + vSL, vO.y + vSL, vO.z + vSL };
					v1.nx = 1; v1.ny = 0; v1.nz = 0;
					v2.nx = 1; v2.ny = 0; v2.nz = 0;
					v3.nx = 1; v3.ny = 0; v3.nz = 0;
					v4.nx = 1; v4.ny = 0; v4.nz = 0;
					v5.nx = 1; v5.ny = 0; v5.nz = 0;
					v6.nx = 1; v6.ny = 0; v6.nz = 0;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;

				}

				// Generate the vertices for the 2 triangles of the -Z face
				if (z == 0 || chunk[x][y][z-1].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x + vSL, vO.y, vO.z };
					Vertex v2{ vO.x, vO.y, vO.z };
					Vertex v3{ vO.x, vO.y + vSL, vO.z };

					Vertex v4{ vO.x, vO.y + vSL, vO.z };
					Vertex v5{ vO.x + vSL, vO.y + vSL, vO.z };
					Vertex v6{ vO.x + vSL, vO.y, vO.z };
					v1.nx = 0; v1.ny = 0; v1.nz = -1;
					v2.nx = 0; v2.ny = 0; v2.nz = -1;
					v3.nx = 0; v3.ny = 0; v3.nz = -1;
					v4.nx = 0; v4.ny = 0; v4.nz = -1;
					v5.nx = 0; v5.ny = 0; v5.nz = -1;
					v6.nx = 0; v6.ny = 0; v6.nz = -1;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;

				}

				// Generate the vertices for the 2 triangles of the +Z face
				if (z == chunkLength - 1 || chunk[x][y][z + 1].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x, vO.y, vO.z + vSL};
					Vertex v2{ vO.x + vSL, vO.y, vO.z + vSL};
					Vertex v3{ vO.x + vSL, vO.y + vSL, vO.z + vSL};

					Vertex v4{ vO.x, vO.y, vO.z + vSL };
					Vertex v5{ vO.x + vSL, vO.y + vSL, vO.z + vSL };
					Vertex v6{ vO.x, vO.y + vSL, vO.z + vSL };

					v1.nx = 0; v1.ny = 0; v1.nz = 1;
					v2.nx = 0; v2.ny = 0; v2.nz = 1;
					v3.nx = 0; v3.ny = 0; v3.nz = 1;
					v4.nx = 0; v4.ny = 0; v4.nz = 1;
					v5.nx = 0; v5.ny = 0; v5.nz = 1;
					v6.nx = 0; v6.ny = 0; v6.nz = 1;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;

				}

				// Generate the vertices for the 2 triangles of the -Y face
				if (y == 0 || chunk[x][y - 1][z].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x, vO.y, vO.z };
					Vertex v2{ vO.x + vSL, vO.y, vO.z };
					Vertex v3{ vO.x + vSL, vO.y, vO.z + vSL };

					Vertex v4{ vO.x, vO.y, vO.z };
					Vertex v5{ vO.x + vSL, vO.y, vO.z + vSL };
					Vertex v6{ vO.x, vO.y, vO.z + vSL };

					v1.nx = 0; v1.ny = -1; v1.nz = 0;
					v2.nx = 0; v2.ny = -1; v2.nz = 0;
					v3.nx = 0; v3.ny = -1; v3.nz = 0;
					v4.nx = 0; v4.ny = -1; v4.nz = 0;
					v5.nx = 0; v5.ny = -1; v5.nz = 0;
					v6.nx = 0; v6.ny = -1; v6.nz = 0;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;

				}

				// Generate the vertices for the 2 triangles of the +Y face
				if (y == chunkHeight - 1 || chunk[x][y + 1][z].type == EMPTY_SPACE)
				{
					Vertex v1{ vO.x, vO.y + vSL, vO.z + vSL };
					Vertex v2{ vO.x + vSL, vO.y + vSL, vO.z + vSL };
					Vertex v3{ vO.x + vSL, vO.y + vSL, vO.z };

					Vertex v4{ vO.x, vO.y + vSL, vO.z + vSL };
					Vertex v5{ vO.x + vSL, vO.y + vSL, vO.z };
					Vertex v6{ vO.x, vO.y + vSL, vO.z };

					v1.nx = 0; v1.ny = 1; v1.nz = 0;
					v2.nx = 0; v2.ny = 1; v2.nz = 0;
					v3.nx = 0; v3.ny = 1; v3.nz = 0;
					v4.nx = 0; v4.ny = 1; v4.nz = 0;
					v5.nx = 0; v5.ny = 1; v5.nz = 0;
					v6.nx = 0; v6.ny = 1; v6.nz = 0;
					FirstAttemptedMesh.vertices.push_back(v1);
					FirstAttemptedMesh.vertices.push_back(v2);
					FirstAttemptedMesh.vertices.push_back(v3);
					FirstAttemptedMesh.vertices.push_back(v4);
					FirstAttemptedMesh.vertices.push_back(v5);
					FirstAttemptedMesh.vertices.push_back(v6);
					NumTrianglesRendered += 2;

				}
			}

	if (NumTrianglesRendered > 0)
	FirstAttemptedMesh.SendToGPU();
	std::cout << "verts: " << FirstAttemptedMesh.vertices.size() << std::endl;


}



void genVertsForBounds(Bounds * bounds, Vertex * vertices) {


	int i = 0;
	float sl = 1.0;
	// Generate the vertices for the 2 triangles of the -X face
	{
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->minZ, -1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->maxZ, -1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->maxZ, -1, 0, 0 };

		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->minZ, -1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->maxZ, -1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->minZ, -1, 0, 0 };
	}
	// Generate the vertices for the 2 triangles of the +X face
	{
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->minZ, 1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->minZ, 1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->maxZ, 1, 0, 0 };

		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->maxZ, 1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->minZ, 1, 0, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->maxZ, 1, 0, 0 };
	}
	// Generate the vertices for the 2 triangles of the -Z face
	{
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->minZ, 0, 0, -1 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->minZ, 0, 0, -1 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->minZ, 0, 0, -1 };

		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->minZ, 0, 0, -1 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->minZ, 0, 0, -1 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->minZ, 0, 0, -1 };
	}

	// Generate the vertices for the 2 triangles of the +Z face
	{
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->maxZ, 0, 0, 1 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->maxZ, 0, 0, 1 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->maxZ, 0, 0, 1 };

		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->maxZ, 0, 0, 1 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->maxZ, 0, 0, 1 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->maxZ, 0, 0, 1 };
	}

	// Generate the vertices for the 2 triangles of the -Y face
	{
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->minZ, 0, -1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->minZ, 0, -1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->maxZ, 0, -1, 0 };

		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->minZ, 0, -1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->minY, bounds->maxZ, 0, -1, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->minY, bounds->maxZ, 0, -1, 0 };
	}

	// Generate the vertices for the 2 triangles of the +Y face
	{
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->maxZ, 0, 1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->maxZ, 0, 1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->minZ, 0, 1, 0 };

		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->maxZ, 0, 1, 0 };
		*(vertices + i++) = Vertex{ bounds->maxX, bounds->maxY, bounds->minZ, 0, 1, 0 };
		*(vertices + i++) = Vertex{ bounds->minX, bounds->maxY, bounds->minZ, 0, 1, 0 };
	}

}












/*
void marchCubes() {

	// Phase 1 and 2

	FastNoiseLite fastNoise((int)(Seed));
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	for (int x = 0; x < chunkWidth; x++)
		for (int y = 0; y < chunkHeight; y++)
			for (int z = 0; z < chunkLength; z++) {
				Voxel* voxel = &chunk[x][y][z];
				voxel->density = fastNoise.GetNoise((float)x * 6, (float)y * 6, (float)z * 6);
				voxel->start = vec3(x, y, z);
				voxel->culled = false;
				if (voxel->density >= PerlinThreshold1) voxel->type = STRUCTURAL;
				else {
					voxel->type = EMPTY_SPACE;
					voxel->culled = true;
				}
			}


	// Phase 3 - NAIVE. Just generating all faces for every cube.

	// Clear the existing vertices if necessary
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.vertices.clear();


	for (int x = 0; x < chunkWidth; ++x) 
		for (int y = 0; y < chunkHeight; ++y) 
			for (int z = 0; z < chunkLength; ++z) {
				//if (chunk[x][y][z].culled) continue;
				vec3 startPos = chunk[x][y][z].start;
				int cubeIndex = 0;
				float vals[8];
				for (int i = 0; i < 8; ++i) {
					vec3 cornerPosition = startPos + corners[i];
					float nz = fastNoise.GetNoise(cornerPosition.x * 48, cornerPosition.y*48, cornerPosition.z* 48);
					scalarValues[i] = nz;
					if (nz < MarchingCubeCornerThreshold)cubeIndex |= 1 << i;
				}

				std::vector<int> triangulation
				(std::begin(MarchingCubes::triTable[cubeIndex]), std::end(MarchingCubes::triTable[cubeIndex]));

	
				for (int edgeIndex : triangulation) {
					if (edgeIndex == -1) continue;
					int indexA = MarchingCubes::cornerIndexFromEdge[edgeIndex][0];
					int indexB = MarchingCubes::cornerIndexFromEdge[edgeIndex][1];
					vec3 newPos = (corners[indexA] + corners[indexB]) / 2.0f;
						//(MarchingCubeCornerThreshold - scalarValues[indexA]) * (corners[indexB] - corners[indexA]) /
						//(scalarValues[indexB] - scalarValues[indexA]);
					newPos += startPos;

		
					Vertex vertex{ newPos.x,newPos.y,newPos.z };
					FirstAttemptedMesh.vertices.push_back(vertex);
				}

			}
	if (FirstAttemptedMesh.vertices.size() > 0) {
		FirstAttemptedMesh.SendToGPU();
	}
		
}

*/

glm::vec3 corners[8] = {
	glm::vec3(0,0,0),
	glm::vec3(1,0,0),
	glm::vec3(1,0,1),
	glm::vec3(0,0,1),
	glm::vec3(0,1,0),
	glm::vec3(1,1,0),
	glm::vec3(1,1,1),
	glm::vec3(0,1,1)
};

/* Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
static vec3
interp_vertex(double isolevel, vec3 p1, vec3 p2, double valp1, double valp2)
{
	double mu;
	vec3 p;

	if (abs(isolevel - valp1) < 0.00001)
		return(p1);
	if (abs(isolevel - valp2) < 0.00001)
		return(p2);
	if (abs(valp1 - valp2) < 0.00001)
		return(p1);
	mu = (isolevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return(p);
}

static int
march_one_cube(GRIDCELL * grid, float isolevel, TRIANGLE* triangles)
{
	int i, ntriang;
	int cubeindex;
	vec3 vertlist[12];

	/*
	  Determine the index into the edge table which
	  tells us which vertices are inside of the surface
	*/
	cubeindex = 0;
	if (grid->val[0] < isolevel) cubeindex |= 1;
	if (grid->val[1] < isolevel) cubeindex |= 2;
	if (grid->val[2] < isolevel) cubeindex |= 4;
	if (grid->val[3] < isolevel) cubeindex |= 8;
	if (grid->val[4] < isolevel) cubeindex |= 16;
	if (grid->val[5] < isolevel) cubeindex |= 32;
	if (grid->val[6] < isolevel) cubeindex |= 64;
	if (grid->val[7] < isolevel) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edgeTable[cubeindex] == 0)
		return(0);
	/* Find the vertices where the surface intersects the cube */
	if (edgeTable[cubeindex] & 1)
		vertlist[0] =
		interp_vertex(isolevel, grid->p[0], grid->p[1], grid->val[0], grid->val[1]);
	if (edgeTable[cubeindex] & 2)
		vertlist[1] =
		interp_vertex(isolevel, grid->p[1], grid->p[2], grid->val[1], grid->val[2]);
	if (edgeTable[cubeindex] & 4)
		vertlist[2] =
		interp_vertex(isolevel, grid->p[2], grid->p[3], grid->val[2], grid->val[3]);
	if (edgeTable[cubeindex] & 8)
		vertlist[3] =
		interp_vertex(isolevel, grid->p[3], grid->p[0], grid->val[3], grid->val[0]);
	if (edgeTable[cubeindex] & 16)
		vertlist[4] =
		interp_vertex(isolevel, grid->p[4], grid->p[5], grid->val[4], grid->val[5]);
	if (edgeTable[cubeindex] & 32)
		vertlist[5] =
		interp_vertex(isolevel, grid->p[5], grid->p[6], grid->val[5], grid->val[6]);
	if (edgeTable[cubeindex] & 64)
		vertlist[6] =
		interp_vertex(isolevel, grid->p[6], grid->p[7], grid->val[6], grid->val[7]);
	if (edgeTable[cubeindex] & 128)
		vertlist[7] =
		interp_vertex(isolevel, grid->p[7], grid->p[4], grid->val[7], grid->val[4]);
	if (edgeTable[cubeindex] & 256)
		vertlist[8] =
		interp_vertex(isolevel, grid->p[0], grid->p[4], grid->val[0], grid->val[4]);
	if (edgeTable[cubeindex] & 512)
		vertlist[9] =
		interp_vertex(isolevel, grid->p[1], grid->p[5], grid->val[1], grid->val[5]);
	if (edgeTable[cubeindex] & 1024)
		vertlist[10] =
		interp_vertex(isolevel, grid->p[2], grid->p[6], grid->val[2], grid->val[6]);
	if (edgeTable[cubeindex] & 2048)
		vertlist[11] =
		interp_vertex(isolevel, grid->p[3], grid->p[7], grid->val[3], grid->val[7]);

	
	/* Create the triangle */
	ntriang = 0;
	for (i = 0; triTable[cubeindex][i] != -1; i += 3)
	{
		triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i]];
		triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i + 1]];
		triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i + 2]];
		ntriang++;
	}

	return(ntriang);
}









/* Calculate the unit normal at p given two other points
   p1,p2 on the surface. The normal points in the direction
   of p1 crossproduct p2
 */
static vec3
do_plane_normal(vec3 p, vec3 p1, vec3 p2)
{
	vec3 n, pa, pb;
	pa.x = p1.x - p.x;
	pa.y = p1.y - p.y;
	pa.z = p1.z - p.z;
	pb.x = p2.x - p.x;
	pb.y = p2.y - p.y;
	pb.z = p2.z - p.z;
	n.x = pa.y * pb.z - pa.z * pb.y;
	n.y = pa.z * pb.x - pa.x * pb.z;
	n.z = pa.x * pb.y - pa.y * pb.x;
	glm::normalize(n);
	return vec3(n.x, n.y, n.z);
}

void marchExistingData() {
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.vertices.clear();
	int nvc = 0;
	for (int x = 0; x < chunkWidth; x++) {
		for (int y = 0; y < chunkHeight; y++) {
			for (int z = 0; z < chunkLength; z++) {
				GRIDCELL* gridCell = &WORLD_GRID[x][y][z];
				//zeroOutGridCell(gridCell);
				TRIANGLE* triangles = new TRIANGLE[5];
				int numTriangles = march_one_cube(gridCell, PerlinThreshold1, triangles);
				gridCell->numFaces = numTriangles;

				for (int i = 0; i < numTriangles; i++) {
				
					vec3 n = do_plane_normal(triangles[i].p[0], triangles[i].p[1], triangles[i].p[2]);
					float x1 = triangles[i].p[0].x, x2 = triangles[i].p[1].x, x3 = triangles[i].p[2].x;
					float y1 = triangles[i].p[0].y, y2 = triangles[i].p[1].y, y3 = triangles[i].p[2].y;
					float z1 = triangles[i].p[0].z, z2 = triangles[i].p[1].z, z3 = triangles[i].p[2].z;
					FirstAttemptedMesh.vertices.push_back(Vertex{ x1,y1,z1,n.x,n.y,n.z });
					FirstAttemptedMesh.vertices.push_back(Vertex{ x2,y2,z2,n.x,n.y,n.z });
					FirstAttemptedMesh.vertices.push_back(Vertex{ x3,y3,z3,n.x,n.y,n.z });
					gridCell->faces[i].tri = TRIANGLE{ vec3(x1,y1,z1),vec3(x2,y2,z2),vec3(x3,y3,z3) };
					gridCell->faces[i].normal = n;
				}
				delete[5] triangles;
			}
		}
	}
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.SendToGPU();
}
/* Indexing convention:

			 Vertices:                    Edges:

		  4  ______________ 5           ______________
		   /|            /|           /|     4      /|
		  / |         6 / |       7  / |8        5 / |
	  7  /_____________/  |        /______________/  | 9
		|   |         |   |        |   |   6     |   |
		| 0 |_________|___| 1      |   |_________|10_|
		|  /          |  /      11 | 3/     0    |  /
		| /           | /          | /           | / 1
	  3 |/____________|/ 2         |/____________|/
										  2
 */
void cacheVertexNeighborsFor(int x, int y, int z) {
	VERTEX_NEIGHBORS[x][y][z][0] = CELL_CORNER_PAIR{ &WORLD_GRID[x][y][z], 0 };
	VERTEX_NEIGHBORS[x][y][z][1] = CELL_CORNER_PAIR{ &WORLD_GRID[x][y - 1][z], 4 };
	VERTEX_NEIGHBORS[x][y][z][2] = CELL_CORNER_PAIR{ &WORLD_GRID[x-1][y][z-1], 2 };
	VERTEX_NEIGHBORS[x][y][z][3] = CELL_CORNER_PAIR{ &WORLD_GRID[x-1][y-1][z-1], 6 };
	VERTEX_NEIGHBORS[x][y][z][4] = CELL_CORNER_PAIR{ &WORLD_GRID[x-1][y][z], 1 };
	VERTEX_NEIGHBORS[x][y][z][5] = CELL_CORNER_PAIR{ &WORLD_GRID[x-1][y-1][z], 5 };
	VERTEX_NEIGHBORS[x][y][z][6] = CELL_CORNER_PAIR{ &WORLD_GRID[x][y][z-1], 3 };
	VERTEX_NEIGHBORS[x][y][z][7] = CELL_CORNER_PAIR{ &WORLD_GRID[x][y-1][z-1], 7 };
}

namespace Vortex {


	class Timer {
	public:
		std::chrono::steady_clock::time_point frameStart;
		std::chrono::steady_clock::time_point frameEnd;
		std::chrono::steady_clock::time_point engineStart;

		void startFrame() {
			frameStart = std::chrono::steady_clock::now();
		}

		void endFrame() {
			frameEnd = std::chrono::steady_clock::now();
		}

		void startEngine() {
			engineStart = std::chrono::steady_clock::now();
		}

		unsigned int engineTimeMilliseconds() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - engineStart).count();
		}

	};

}
Vortex::Timer timer;
void marchCubesStanford() {
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.vertices.clear();
	FastNoiseLite fastNoise((int)(Seed));
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	FastNoiseLite fastNoise2((int)Seed);
	fastNoise2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	// Assign the grid cells their start vector/ p vector 
	int nvc = 0;
	for (int x = 0; x < chunkWidth; x++) {
		for (int y = 0; y < chunkHeight; y++) {
			for (int z = 0; z < chunkLength; z++) {
				GRIDCELL* gridCell = &WORLD_GRID[x][y][z];

				if (x>0&&x<chunkWidth&&y>0&&y<chunkHeight&&z>0&&z<chunkLength)
					cacheVertexNeighborsFor(x, y, z); // Cache the vertex neighbors
				for (int i = 0; i < 8; i++) {
					vec3 p = vec3(x, y, z) + corners[i];
					gridCell->p[i] = p;
					gridCell->val[i] = p.y < 2 || p.y > chunkHeight - 2 || p.z < 2 || p.z > chunkLength - 2 || p.x < 2 || p.x > chunkWidth - 2 ?
						PerlinThreshold1 + 0.01 :
						//((length((p - vec3(chunkWidth/2, chunkWidth/2, chunkWidth/2))) / (((float)chunkWidth)/1.0f))) - 0.5f
						//;// Distance from a sphere centered at 8,8,8, shifted to be within the -.5 to .5 range
						(p.y >= 8 && p.y <= 14) ? PerlinThreshold1+ .12 - (fastNoise.GetNoise(p.x * 4, p.y * 4, p.z * 7)) : // Creating a space in the middle
						((fastNoise.GetNoise(p.x * 12, p.y * 12, p.z * 12))) * fastNoise2.GetNoise(p.x*5,p.y*5,p.z*8);
				}
				TRIANGLE* triangles = new TRIANGLE[5];
				int numTriangles = march_one_cube(gridCell, PerlinThreshold1, triangles);
				
				gridCell->numFaces = numTriangles;

				for (int i = 0; i < numTriangles; i++) {
					vec3 n = do_plane_normal(triangles[i].p[0], triangles[i].p[1], triangles[i].p[2]);
					float x1 = triangles[i].p[0].x, x2 = triangles[i].p[1].x, x3 = triangles[i].p[2].x;
					float y1 = triangles[i].p[0].y, y2 = triangles[i].p[1].y, y3 = triangles[i].p[2].y;
					float z1 = triangles[i].p[0].z, z2 = triangles[i].p[1].z, z3 = triangles[i].p[2].z;
					FirstAttemptedMesh.vertices.push_back(Vertex{ x1,y1,z1,n.x,n.y,n.z });
					FirstAttemptedMesh.vertices.push_back(Vertex{ x2,y2,z2,n.x,n.y,n.z });
					FirstAttemptedMesh.vertices.push_back(Vertex{ x3,y3,z3,n.x,n.y,n.z });
					gridCell->faces[i].tri = TRIANGLE{ vec3(x1,y1,z1),vec3(x2,y2,z2),vec3(x3,y3,z3) };
					gridCell->faces[i].normal = n;
				}

				delete[5] triangles;
			}
		}

	}

	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.SendToGPU();
}



vec3 preResetForce;


PhysicsRealm::PlaneCollider planeCollider(vec3(0, 1, 0), vec3(0, 0, 0));
PhysicsRealm::SphereCollider sphereCollider(camera.Position, 2.0f);
PhysicsRealm::EuclidianObject playerObject(&sphereCollider, PhysicsRealm::NewtonianAttributeSet{ camera.Position });

bool freezePhysics = true;
void prepIMGUIFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Control GUI");

		ImGui::SetWindowPos(ImVec2(0, 0), 2);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 4.0, ImGui::GetIO().DisplaySize.y / 2.0), 2);
		ImGui::SetWindowCollapsed(true, 2);
		ImGui::SliderFloat("World Gravity", &GRAVITATIONAL_ACCELERATION, 0.0f, 20.0f);
		ImGui::SliderFloat("Player Movement Velocity", &MAX_PLAYER_SPEED, 0.0f, 100.0f);
		ImGui::SliderFloat("Player Jump Acceleration", &MAX_PLAYER_JUMP_ACCELERATION, 0.0f, 200.0f);
		ImGui::SliderFloat("Time Step Divisor", &TIME_STEP_FACTOR, 0.0f, 200.0f);
		ImGui::SliderFloat("Build Strength", &BUILD_STRENGTH, .02	, 2.0f);
		ImGui::SliderFloat("Air Drag", &AIR_DRAG, .005	, 1.0f);
		ImGui::SliderFloat("Build Strength", &FORCE_OF_GRAVITY, 1.0f, 50.0f);
		ImGui::SliderInt("Seed", &Seed, 0, 200);

		if (ImGui::Button(freezePhysics ? "Start Physics" : "Freeze Physics")) {
			freezePhysics = !freezePhysics;
		}
		if (ImGui::SliderFloat("Perlin Threshold 1", &PerlinThreshold1, -1.0f, 1.00f)) {
		}	if (ImGui::SliderFloat("Marching Corner Threshold", &MarchingCubeCornerThreshold, -.50f, 0.50f)) {

		}
		ImGui::SliderFloat("Camera Zoom", &camera.Zoom, 0.0, 140.0f);

		float FPS = ImGui::GetIO().Framerate;
		if (ImGui::CollapsingHeader("Metrics")) {
			ImGui::Text("Metrics");
			ImGui::Text(to_string(camera.Position).c_str());
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 2000.0f / FPS, FPS);
		}

		if (ImGui::Button("Rebuild")) {
			if (!ApplyRLE) marchCubesStanford();
			else Phase1And2And3();
			ApplyRLE = !ApplyRLE;

		}

		ImGui::Text(to_string(preResetForce).c_str());

		ImGui::End();
		ImGuiWindowFlags window_flags = 0;
		bool closeable = true;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Marching Cubes GUI", &closeable, window_flags);
		ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetIO().DisplaySize.x / 4.0f, 300), 2);
		ImGui::SetWindowSize(ImVec2(120, 250), 2);
		ImGui::SameLine();
		ImGui::End();
	}
}



void alterVertexDensity(int x, int y, int z, float change) {
	for (int i = 0; i < 8; i++) {
		VERTEX_NEIGHBORS[x][y][z][i].cell->val[VERTEX_NEIGHBORS[x][y][z][i].corner] += change;
	}
}

int main() {
	timer.startEngine();
	setupIMGUI();
	glm::mat4 worldMatrix(1.0f);
	marchCubesStanford();
	blockShader.use();
	blockShader.setMat4("transform", *cube.getTransformMatrix());
	blockShader.setMat4("model", glm::mat4(1.0f));
	blockShader.setMat4("projection", *camera.GetProjectionMatrix());
	blockShader.setMat4("world", worldMatrix);
	gridShader.use();
	gridShader.setMat4("transform", *ep.getTransformMatrix());
	gridShader.setMat4("model", glm::mat4(1.0f));
	gridShader.setMat4("projection", *camera.GetProjectionMatrix());
	gridShader.setMat4("world", worldMatrix);
	int cc = 0;

	int MS_PER_FRAME = 16;
	bool lockToBall = true;
	while (!glfwWindowShouldClose(mWind))
	{
		prepIMGUIFrame();


		// AI
		//Where are the AI?
		// How do they know where the player is? 

		// How do they set up a path to head to them (what is their first action)?
		// If he is in sight, what do they do?

		// Collision Engine

		// Collision Detection Phase
		// There are 7 possible voxels for the player to collide with assuming they are 1 voxel in size.
		//GRIDCELL * possibleVoxelsToCollideWith()
		// Lets start with the current voxel.

	
		GRIDCELL* centerVoxel = &WORLD_GRID[min(chunkWidth-1,max(0,(int)playerObject.newtonianAttributeSet.Position.x))][min(chunkHeight-1,max(0,(int)playerObject.newtonianAttributeSet.Position.y))]
			[min(chunkLength-1,max(0,(int)playerObject.newtonianAttributeSet.Position.z))];
		// Bounding sphere around player of .5 radius

		float radius = BUILD_STRENGTH;
		// Let's start with just colliding with faces below us.
		vec3 casts[6]{
			vec3 (0, -1, 0),
			vec3 (0, 1, 0),
			vec3 (-1, 0, 0),
			vec3 (1, 0, 0),
			vec3 (0, 0, 1),
			vec3 (0, 0, -1)
		}
		;
		bool hc = false;
		vec3 n(0,0,0);
		// Lets see if this cast intersects with any of hte faces of the voxel we're in.
		vec3 nearestNorm(0);
		float nearestDist = 9999999999.9f;
		int intersectingCast = 0;
		for (int i = 0; i < centerVoxel->numFaces; i++) {
			for (int j = 0; j < 6; j++) {
				// Check for intersection of ray triangle
				vec2 bary; float dist; 

				intersectRayTriangle(playerObject.newtonianAttributeSet.Position, casts[j],
					centerVoxel->faces[i].tri.p[0], centerVoxel->faces[i].tri.p[1], centerVoxel->faces[i].tri.p[2],
					bary, dist);

				// If the distance is within the bounding radius and the dot product is negative (face is facing outwards) then we have a collision
				float dotProduct = dot(centerVoxel->faces[i].normal, casts[j]);
				if (dist >= 0)
				if (dist < radius && dotProduct <= 0) {
					if (dist < nearestDist) {
						hc = true;
						n = centerVoxel->faces[i].normal;
						nearestDist = dist;
						intersectingCast = j;
					}
				}
			}
		}
		//if (hc) marchExistingData();

			

		// Physics Engine
		
		if (hc) {
			alterVertexDensity(
				(int)playerObject.newtonianAttributeSet.Position.x,
				(int)playerObject.newtonianAttributeSet.Position.y,
				(int)playerObject.newtonianAttributeSet.Position.z,
				.05f);
			//marchExistingData();
		}


		// All entities are affected by graviity
		// Collision Response Phase


		// TERRAIN COLLISION
		// If you are an nearing a terrain facing, ther eneeds to be a stopping force along the normal of the face in relation to the collision force/velocity/etc. 

		float dt_seconds = 1.0f / ImGui::GetIO().Framerate;

		Sphere spherey(BUILD_STRENGTH);

		preResetForce = playerObject.Force_Integration_Step(dt_seconds / TIME_STEP_FACTOR);
		if (lockToBall)camera.Position = playerObject.newtonianAttributeSet.Position;// -1.0f * camera.Front;
		spherey.setTransform(translate(playerObject.newtonianAttributeSet.Position));
		// Process Inputs
		glfwPollEvents();
		IM.processInput();
		if (IM.spacePressed && hc) {
			playerObject.newtonianAttributeSet.Force +=  vec3(0, 200.0f, 0);
			IM.spacePressed = false;
		}
		if (IM.wPressed) playerObject.newtonianAttributeSet.Force += vec3(camera.Front.x,camera.Front.y/2.0f,camera.Front.z) * 2.0f;
		if (IM.qPressed) playerObject.newtonianAttributeSet.Force += vec3(0, 20.0f, 0);
		camera.UpdateProjection();

		// Game Rendering
		mainWindow->clearColor(0, 0.08, 0.18, 1.0);// (.24, 0.56, 0.77, 1.0);
		blockShader.use();
		blockShader.setMat4("view", camera.GetViewMatrix());
		blockShader.setMat4("transform", mat4(1.0f));
		blockShader.setMat4("projection", *camera.GetProjectionMatrix());
		blockShader.setVec3("PlayerPos", playerObject.newtonianAttributeSet.Position);
		if (FirstAttemptedMesh.vertices.size() > 0)
			FirstAttemptedMesh.Draw(IM.primitiveType);

		Line l(camera.Position, IM.rayDirection*100.0f);
		
		blockShader.setMat4("transform", *spherey.getTransformMatrix());
		//spherey.Draw(IM.primitiveType);
		// GUI Rendering


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());




		// Buffer swap
		glfwSwapBuffers(mWind);
	}

	glfwTerminate();

	return -1;

}


