#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <engine/core/AssetManager.hpp>
#include <engine/core/Renderer.hpp>
#include <engine/core/Instancer.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/modules/Camera.hpp>
#include <engine/modules/InstanceRenderer.hpp>
#include <engine/modules/Renderable.hpp>

#include <engine/utils/Utils.hpp>

using namespace std;
using namespace Engine::Core;
using namespace Engine::Module;
//using namespace Engine::Data;

// TODO: shortcut types -> String should be const &
//using String = std::string const &;

class Sprite3D : public Renderable {
public:
	Sprite3D (IRenderer & renderer) {
		SetRenderer (renderer);


		auto * _assetManager = AssetManager::GetInstance ();
		SetModel (_assetManager->GetQuadModelID ());
	}

	Sprite3D (IRenderer & renderer, const HandleID & texture) {
		SetRenderer (renderer);
		
		auto * _assetManager = AssetManager::GetInstance ();
		SetModel (_assetManager->GetQuadModelID ());
		GetModel ().meshes[0].material.SetTexture (texture);
	}
};


int main () {

	// TODO : add this to the engine
	#ifdef __linux__
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow * window = glfwCreateWindow (800, 800, "TETO ENGINE", NULL, NULL);
	if (!window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit (-1);
	}

	enableOpenGLDebugCallback ();

	glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace (GL_CCW);

	glViewport (0, 0, 800, 800);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glfwSwapInterval (0);

	stbi_set_flip_vertically_on_load(0);

	AssetManager * am = AssetManager::GetInstance ();

	InstanceRenderer renderer {800, 800};
	Camera cam;

	HandleID _defaultShader = am->LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	HandleID badalisc_tex = am->LoadTexture ("badalisc", "data/assets/textures/badalisc_placeholder.png");

	HandleID deny_ace = am->LoadTexture ("deny_ace", "data/assets/textures/card_deny_ace.png");
	HandleID deny_four = am->LoadTexture ("deny_4", "data/assets/textures/card_deny_4.png");

	Sprite3D sprite {renderer, deny_four};
	sprite.SetShader (_defaultShader);
	
	// mouse stuff
	// TODO: add to engine
	double SENSITIVITY = 0.25f;
	double mouse_x = 0.0;
	double mouse_y = 0.0;
	double last_x = 0.0;
	double last_y = 0.0;
	
	// mesuring stuff + FPS
	long long unsigned int c = 0;
	double lastRenderTime	= 0.0;
	double lastTime		= 0.0;
	double currentTime	= lastRenderTime;
	double deltaTime	= 0.0;
	double targetFPS = 144.0;
	double targetFrameTime = 1.0 / targetFPS;

	// TODO-make : add render loop to engine
	while (!glfwWindowShouldClose (window)) {
		currentTime = glfwGetTime ();
		deltaTime = currentTime - lastTime;

		glfwGetCursorPos (window, &mouse_x, &mouse_y);
		cam.rotate ({(mouse_x - last_x) * SENSITIVITY, (mouse_y - last_y) * -SENSITIVITY, 0.0});

		if (glfwGetKey (window, GLFW_KEY_W) == GLFW_PRESS) {
			cam.move_forward (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_S) == GLFW_PRESS) {
			cam.move_backward (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_A) == GLFW_PRESS) {
			cam.move_left (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_D) == GLFW_PRESS) {
			cam.move_right (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_E) == GLFW_PRESS) {
			cam.move_up (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_Q) == GLFW_PRESS) {
			cam.move_down (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose (window, true);
		}

		if (glfwGetKey (window, GLFW_KEY_P) == GLFW_PRESS) {
			cam.rotation.x = 90.0;
			cam.rotation.y = 10.0;
		}

		last_x = mouse_x;
		last_y = mouse_y;

		
		if (currentTime - lastRenderTime < targetFrameTime) {
			glfwPollEvents ();
			lastTime = currentTime;
			continue;
		}

		//clock_t renderingModelsDelta = std::clock ();
		
		// Render here :
		sprite.Render ();

		//renderingModelsDelta = std::clock () - renderingModelsDelta;
		//if (c % 100 == 0) std::cout << "Model draw call took " << renderingModelsDelta << "ms\n";
		++c;
		renderer.Render (&cam);

		glfwSwapBuffers(window);
		glfwPollEvents ();

		lastRenderTime	= currentTime;
		lastTime	= currentTime;

	}

	return 0;
}