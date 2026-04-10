#include <MVP/presenters/CombatPresenter.hpp>

#include <opengl/RenderContext.hpp>

int maim ()
{
	TLOT::RenderContext ctx;
	ctx.Init (800, 800, "Combat Test");

	CombatPresenter presenter {}; // give args to construct view and model, but irrelevant in this proof of concept for now
	CombatView * view = presenter.GetView ();

	ctx.SetUpdateLoop ([&] (TLOT::RenderContext::Context const & ctx) {
		view->Update (ctx);
	});

	ctx.SetRenderLoop ([&] (TLOT::RenderContext::Context const & ctx) {
		// somehow the Syntax will kinda look like that
		//frame = transparentObjectRenderer.RenderTo ();
		//baseObjectRenderer.RenderFromTo (frame, compositor);
		//compositor.ApplyAllPostProcess ();
	});

	return 0;
}