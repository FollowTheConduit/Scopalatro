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


class RenderableAccessor : public Renderable {
public:
	void SetRendererR (IRenderer & renderer)    { SetRenderer (renderer); }
	void SetModelR    (const HandleID & model)  { SetModel (model); };
	void SetShaderR   (const HandleID & shader) { SetShader (shader); };

	void Translate (glm::vec3 pos) {
		GetModel ().Translate (pos);
		GetRenderer ().UpdateModelTransform (GetModel ());
	}

	void Rotate (glm::vec3 rot) {
		GetModel ().Rotate (rot);
		GetRenderer ().UpdateModelTransform (GetModel ());
	}

	void Scale (glm::vec3 scale) {
		GetModel ().Scale (scale);
		GetRenderer ().UpdateModelTransform (GetModel ());
	}
};

int main () {
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

	cout << "Hello World !\n";

	InstanceRenderer renderer {800, 800};

	AssetManager * am = AssetManager::GetInstance ();


	HandleID default_shader = am->LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	
	std::vector<RenderableAccessor> models;
	for (const auto & url : {"data/assets/models/columbina/scene.gltf"}) {
		std::cout << url << "\n";
		for (int j = 0; j < 4; ++j) {
			RenderableAccessor _temp;
			_temp.SetRendererR (renderer);
			_temp.SetModelR (am->LoadModel (url, url));
			_temp.SetShaderR (default_shader);

			models.push_back (_temp);
		}
	}

	models[1].Translate ({1.0, 0.0, 0.0});
	models[2].Translate ({0.0, 2.0, 0.0});
	models[3].Translate ({0.0, 0.0, 1.0});
	
	HandleID edge = am->LoadShader ("edge", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/edge.postprocess.fragment");
	//HandleID gray    = am->LoadShader ("gray", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/grayscale.postprocess.fragment");
	HandleID bnw     = am->LoadShader ("bnw", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/black_and_white.postprocess.fragment");
	HandleID inverse = am->LoadShader ("inverse", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/inverse.postprocess.fragment");
	//HandleID render_input_test = am->LoadShader ("rit", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/test.postprocess.fragment");
	Camera cam;

	//renderer.AddPostProcessingEffect ("gray",	gray);
	//renderer.AddPostProcessingEffect ("edge",	edge);
	//renderer.AddPostProcessingEffect ("bnw",	bnw);
	//renderer.AddPostProcessingEffect ("inverse",	inverse);

	//glm::vec3 color = {1.0f, 0.f, 0.f};

	double SENSITIVITY = 0.25f;
	double mouse_x = 0.0;
	double mouse_y = 0.0;
	double last_x = 0.0;
	double last_y = 0.0;
	
	double lastRenderTime	= 0.0;
	double lastTime		= 0.0;
	double currentTime	= lastRenderTime;
	double deltaTime	= 0.0;
	
	double targetFPS = 144.0;
	double targetFrameTime = 1.0 / targetFPS;


	//ShaderInputs inputs;
	//inputs.AddInput<glm::vec3> ("uColor", [&color] () -> const glm::vec3 & { return (color); });
	//renderer.AddPostProcessingEffect ("rit", render_input_test, 1, std::move (inputs));

	long long unsigned int c = 0;

	while (!glfwWindowShouldClose (window)) {
		currentTime = glfwGetTime ();
		deltaTime = currentTime - lastTime;

		models[2].Rotate ({deltaTime * 100, 0.0, 0.0});
		models[3].Rotate ({0.0, deltaTime * 100, 0.0});
		models[1].Rotate ({0.0, 0.0, deltaTime * 100});

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

		clock_t renderingModelsDelta = std::clock ();
		for (auto & model : models) {
			model.Render ();
		}
		renderingModelsDelta = std::clock () - renderingModelsDelta;
		if (c % 100 == 0) std::cout << "Model draw call took " << renderingModelsDelta << "ms\n";
		++c;
		//printf ("Currently Rendering\n");
		renderer.Render (&cam);

		glfwSwapBuffers(window);
		glfwPollEvents ();

		lastRenderTime	= currentTime;
		lastTime	= currentTime;

	}

	return 0;
}