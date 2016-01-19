#include "fbgfx_view.h"
#include "debug.h"

FBGfx::View::View(bool _context_guarenteed)
{
	//
}

FBGfx::View::View()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (!FBGfx::has_context)
	{
		DEBUG_ERROR("Cannot construct View before calling screen.");
	}
#endif
}

void FBGfx::View::SetViewport(int _x0, int _y0, int _x1, int _y1)
{
	viewport = FBGfx::Quad(_x0, _y0, _x1, _y1);
}

void FBGfx::View::SetViewport(const FBGfx::Quad _view)
{
	viewport = _view;
}