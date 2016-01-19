#pragma once
#include "fbgfx_view.h"

class FBGfx::ViewOrthographic : public FBGfx::View
{
	friend class FBGfx;
public:
	ViewOrthographic();
	void MoveTo(const FBGfx::Point _delta);
	void SizeTo(float _width, float _height);
	void RotateTo(float _z_theta);
	void RotateTo(float _z_theta, const FBGfx::Point _origin);
	void RotateTo(float _z_theta, float _ox, float _oy);
private:
	void compute_projection();
	void reset();
	ViewOrthographic(bool _context_guarenteed);
	FBGfx::Point center;
	float half_width;
	float half_height;

	glm::fmat4 ortho_matrix;
	glm::fmat4 rotation_matrix;
};

