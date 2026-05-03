#include <MVP/presenters/CombatPresenter.hpp>

#include <opengl/RenderContext.hpp>

#include <modules/Camera.hpp>

using namespace TLOT;

int maim ()
{
	RenderContext ctx;
	ctx.Init (800, 800, "Combat Test");

	AssetManager assetManager;
	Renderer actorRenderer;

	RenderableManager actorManager {assetManager, actorRenderer};

	std::unique_ptr<Card> dummy = std::make_unique<Card> ("Dummy Card", "If you see this, there is a bug.");

	CombatParams params;
	params.enemies = {};
	params.playerCards = {dummy.get ()};
	params.playerHP = 50.0;
	params.playerMaxHP = 50.0;
	params.playerItems = 0; // TODO: placeholder

	
	ResourceHandle shaderHandle = assetManager.LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	Shader const & shader = assetManager.GetShader (shaderHandle);
	
	Camera cam;

	CombatPresenter presenter {params, actorManager, cam, ctx}; // give args to construct view and model, but irrelevant in this proof of concept for now
	CombatView * view = presenter.GetView ();

	ctx.SetUpdateLoop ([&] (TLOT::RenderContext::Context const & ctx) {
		view->Update (ctx, cam);
	});

	ctx.SetRenderLoop ([&] (TLOT::RenderContext::Context const & ctx) {
		// TODO: a renderable should have a shader to pass to the renderer
		// TODO: a shader should not be an asset
		// The assetManager should load "ShaderSource"
		// and the renderer should create the "Shader"
		// by combining multiple "ShaderSource"s
		actorRenderer.Render (shader, cam);

		// somehow the Syntax will kinda look like that
		//frame = transparentObjectRenderer.RenderTo ();
		//baseObjectRenderer.RenderFromTo (frame, compositor);
		//compositor.ApplyAllPostProcess ();
	});

	return 0;
}