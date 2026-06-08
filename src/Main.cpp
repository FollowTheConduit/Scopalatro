#include <memory>

#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>
#include <AssetLoader.hpp>

#include <RenderableObjects/CardModel.hpp>
#include <RenderableObjects/HealthbarModel.hpp>

#include <Debugger/DebugImGuI.hpp>
#include <Debugger/SceneInspector.hpp>
#include <Debugger/DebugRenderer.hpp>

#include <chrono>

#include <Game.hpp>

using namespace TLOT;

int main()
{
	size_t width = 1280;
	size_t height = 800;

	RenderContext ctx;
	ctx.Init (width, height, "Scopalatro");
	// ctx and renderer will probably become singleton
	// TODO: see what can be done about multithreading
	// maybe put the atlas creation in the asset manager ?
	// then the renderer won't have the responsability to provide quads anymore ?

	// cache it in assetManager
	std::shared_ptr<VertexTemplate> vertexTemplate = std::make_shared<VertexTemplate>(std::vector<VertexAtom::Type> {
		VertexAtom::Vec3,
		VertexAtom::Vec2
	});

	SceneInspector inspector;
	Renderer renderer {vertexTemplate, width, height}; // renderer should be in the context class (probabky not constructed in place tho)
	// + vertexTemplate should not be a 'dependency' for renderer, there should probably be multiple templates allowed
	
	inspector.SetRenderer(&renderer);

	Game game {&ctx, &renderer, &inspector, vertexTemplate};

	int count = 0;

	int updateCount = 0;
	SetupImGuI (ctx.window);
	while (ctx.ShouldRenderFrame() || !ctx.ShouldExit())
	{
		ctx.Update ();
		updateCount++;
		
		auto begin = std::chrono::steady_clock::now();
		game.Update(ctx.deltaTime);
		auto end   = std::chrono::steady_clock::now();

		++count;
		
		if (count > 1000)
		{
			auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
			//Logger::log(LogLevel::Info, "Time difference = {} [µs]", delta);
			count = 0;
		}

		if (!ctx.ShouldRenderFrame ()) continue;
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // will be part of the technique class responsabilty in a future update

		game.Render();

		renderer.Render();

		DebugRenderer::Get().Render();

		StartFrameImGuI();
		inspector.OpenMenu();
		EndFrameImGuI();

		ctx.Render ();

		//Logger::log(LogLevel::Info, "Executed : {} updates since last frame.", updateCount);
		updateCount = 0;

	}

	return 0;
}