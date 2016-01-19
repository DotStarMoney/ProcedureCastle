#pragma once
#include "fbgfx.h"
#include "glm\mat4x4.hpp"

// should this be overloaded into 2D and 3D?
// ViewOrtho
// ViewPerspective

class FBGfx::View
{
public:
	void SetViewport(int _x0, int _y0, int _x1, int _y1);
	void SetViewport(const FBGfx::Quad _view);
protected:
	View(bool _context_guarenteed);
	View();
	FBGfx::Quad viewport;
	glm::fmat4 projection_matrix;
};

