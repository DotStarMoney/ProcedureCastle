#pragma once
#include "fbgfx_view.h"

// TODO

class FBGfx::ViewPerspective : public FBGfx::View
{
	friend class FBGfx;
public:
	ViewPerspective();
private:
	void compute_projection();
	ViewPerspective(bool _context_guarenteed);
};

