#include <iostream>
#include <chrono>
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
#include <Meshes/FreeMesh.h>

using namespace glm;

// Physics definitions
float GRAVITATIONAL_ACCELERATION = 0.0f;
float MAX_PLAYER_SPEED = 25.0f;
float MAX_PLAYER_JUMP_ACCELERATION = 9.81f;
float TIME_STEP_FACTOR = 1.0f;

float PerlinThreshold1 = .095f;

// Constants for defining window size
const float SCR_WIDTH = 1920.0, SCR_HEIGHT = 1080.0;

OpenGLWindow * mainWindow = new OpenGLWindow(SCR_WIDTH, SCR_HEIGHT);
GLFWwindow * mWind = mainWindow->glWindow();
Camera camera(mainWindow, glm::vec3(0, 3, 25));
Shader gridShader("Shaders/gridShader.vs", "Shaders/gridShader.fs");
Shader blockShader("Shaders/blockShader.vs", "Shaders/blockShader.fs");

EditablePlane ep(50, 50);
UI_InputManager IM(mWind, &camera);

void mouseGridInteract()
{
	if (IM.leftMouseDown) {
		EditablePlane::Vertex3* test = ep.getNearestVertexToIntersectingRay(camera.Position, IM.rayDirection);
		if (test != nullptr) {
			test->y += IM.spacePressed ? -0.50 : 0.1;
			test->state += 0.05f;
			ep.sendVerticesToGPU();
		}
	}
}


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

void sendNoiseToGrid(int seed) {

	FastNoiseLite fastNoise(seed);
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	for (int x = 0; x < 200; x++)
		for (int z = 0; z < 200; z++)
			ep.vertices[x][z].state = fastNoise.GetNoise((float)x, (float)z);
	ep.sendVerticesToGPU();

}




struct Voxel {
	vec3 start;
	float density;
	int type;
	bool culled = false;
};

Voxel chunk[16][16][16];

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

	FastNoiseLite fastNoise(444);
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 16; y++)
			for (int z = 0; z < 16; z++) {
				Voxel* voxel = &chunk[x][y][z];
				voxel->density = fastNoise.GetNoise((float)x * 10, (float)y * 10, (float)z * 10);
				voxel->start = vec3(x, y, z);
				if (voxel->density >= PerlinThreshold1) voxel->type = STRUCTURAL;
				else voxel->type = EMPTY_SPACE;
			}


	// Phase 3 - NAIVE. Just generating all faces for every cube.
	
	// Clear the existing vertices if necessary
	if (!FirstAttemptedMesh.vertices.empty())
		FirstAttemptedMesh.vertices.clear();

	float vSL = 1.0f; // Voxel Side Length
	for (int x = 0; x < 16; x++) 
		for (int y = 0; y < 16; y++)
			for (int z = 0; z < 16; z++) {
				//if (x != 0 || y != 0 || z != 0) continue; // Isolates only one cube - for testing purposes

				// This Voxel
				Voxel * voxel = &chunk[x][y][z];

				if (voxel->type == EMPTY_SPACE) continue;

				vec3 vO = voxel->start; // Voxel Origin

				// Generate the vertices for the 2 triangles of the -X face
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
				}
				// Generate the vertices for the 2 triangles of the +X face
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
				}

				// Generate the vertices for the 2 triangles of the -Z face
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
				}

				// Generate the vertices for the 2 triangles of the +Z face
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
				}

				// Generate the vertices for the 2 triangles of the -Y face
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
				}

				// Generate the vertices for the 2 triangles of the +Y face
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
				}
			}
	FirstAttemptedMesh.SendToGPU();


}

void prepIMGUIFrame() {
	// Start Dear ImGui 
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Control GUI");

		ImGui::SetWindowPos(ImVec2(0, 0), 2);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 4.0, ImGui::GetIO().DisplaySize.y / 2.0), 2);
		ImGui::SetWindowCollapsed(true, 2);
		ImGui::SliderFloat("World Gravity", &GRAVITATIONAL_ACCELERATION, 0.0f, 20.0f);
		ImGui::SliderFloat("Player Movement Velocity", &MAX_PLAYER_SPEED, 0.0f, 500.0f);
		ImGui::SliderFloat("Player Jump Acceleration", &MAX_PLAYER_JUMP_ACCELERATION, 0.0f, 200.0f);
		ImGui::SliderFloat("Time Step Divisor", &TIME_STEP_FACTOR, 0.0f, 200.0f);
		ImGui::SliderFloat("Perlin Threshold 1", &PerlinThreshold1, -.50f, 0.50f);
		ImGui::SliderFloat("Camera Zoom", &camera.Zoom, 0.0, 100.0f);

		float FPS = ImGui::GetIO().Framerate;
		if (ImGui::CollapsingHeader("Metrics")) {
			ImGui::Text("Metrics");
			ImGui::Text(to_string(camera.Position).c_str());
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 2000.0f / FPS, FPS);
		}

		if (ImGui::Button("Re-Seed Noise")) {
			Phase1And2And3();
			//sendNoiseToGrid((int)(camera.Position.y + camera.Position.x));
		}
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
int main() {

	setupIMGUI();
	glm::mat4 worldMatrix(1.0f);


	int originalCount = 0;

	std::vector<Voxel> denseVoxels;

	Phase1And2And3();








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


	while (!glfwWindowShouldClose(mWind))
	{
		prepIMGUIFrame();


		mainWindow->clearColor(.36, 0.41, 0.52, 1.0);

		gridShader.use();
		gridShader.setMat4("view", camera.GetViewMatrix());
		gridShader.setMat4("projection", *camera.GetProjectionMatrix());

		ep.Draw(IM.primitiveType);

		blockShader.use();
		blockShader.setMat4("view", camera.GetViewMatrix());
		blockShader.setMat4("projection", *camera.GetProjectionMatrix());
		FirstAttemptedMesh.Draw(IM.primitiveType); 
		//iterateOnChunkVoxels(drawCube);
		
		float dt_seconds = 1.0f / ImGui::GetIO().Framerate ;
		camera.physicsUpdate(dt_seconds/TIME_STEP_FACTOR);
		camera.UpdateProjection();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		IM.processInput(dt_seconds);
		glfwSwapBuffers(mWind);
	}

	glfwTerminate();

	return -1;

}


