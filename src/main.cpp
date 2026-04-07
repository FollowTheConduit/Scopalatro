#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <modules/AssetManager.hpp>

#include <opengl/RenderContext.hpp>

#include <modules/Camera.hpp>
#include <modules/Renderer.hpp>
#include <modules/Renderable.hpp>

#include <renderables/Card.hpp>

using namespace std;
using namespace TLOT;


int main ()
{
	RenderContext context {800, 800, "Scopalatro"};

	AssetManager assetManager;
	Renderer renderer;

	RenderableManager rManager {assetManager, renderer};
	Camera camera;
	
	CardManager cardManager {rManager};

	cardManager.Create (CardColor::Deny, CardSymbol::Ace);

	ResourceHandle shaderHandle = assetManager.LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	Shader const & shader = assetManager.GetShader (shaderHandle);


	static float const SENSITIVITY = 0.25f;

	context.SetUpdateLoop ([&](RenderContext::Context const & context) {
		double deltaX = context.mouseX - context.lastMouseX;
		double deltaY = context.mouseY - context.lastMouseY;

		camera.rotate ({deltaX * SENSITIVITY, deltaY * -SENSITIVITY, 0.0});

		if (glfwGetKey (context.window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.move_up (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.move_down (context.deltaTime);
		}

		if (glfwGetKey (context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose (context.window, true);
		}

		if (glfwGetKey (context.window, GLFW_KEY_P) == GLFW_PRESS) {
			camera.rotation.x = 90.0;
			camera.rotation.y = 10.0;
		}
	});

	context.SetRenderLoop ([&](RenderContext::Context const & context) {
		renderer.Render (shader, camera);
	});

	context.Loop ();

	return 0;
}