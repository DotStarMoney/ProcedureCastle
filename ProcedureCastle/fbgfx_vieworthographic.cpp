#include "fbgfx_vieworthographic.h"
#include "glm\gtc\matrix_transform.hpp"
#include <float.h>

FBGfx::ViewOrthographic::ViewOrthographic(bool _context_guarenteed) :
	FBGfx::ViewOrthographic(_context_guarenteed)
{
	//
}

FBGfx::ViewOrthographic::ViewOrthographic() : FBGfx::View()
{
	reset();
}

void FBGfx::ViewOrthographic::reset()
{
	half_width = (float)FBGfx::window.scrx * 0.5f;
	half_height = (float)FBGfx::window.scry * 0.5f;
	ortho_matrix = glm::ortho(
		-half_width,
		-half_height,
		half_width,
		half_height);
	rotation_matrix = glm::mat4(1.0);
	compute_projection();
	SetViewport(0, 0, FBGfx::window.scrx, FBGfx::window.scry);
}

void FBGfx::ViewOrthographic::compute_projection()
{
	projection_matrix = rotation_matrix * ortho_matrix;
}

void FBGfx::ViewOrthographic::MoveTo(const FBGfx::Point _delta)
{
	center = _delta;
	ortho_matrix = glm::ortho(
		center.x - half_width,
		center.y - half_height,
		center.x + half_width,
		center.y + half_height);
	compute_projection();
}

void FBGfx::ViewOrthographic::SizeTo(float _width, float _height)
{
	half_width = _width * 0.5f;
	half_height = _height * 0.5f;
	ortho_matrix = glm::ortho(
		center.x - half_width,
		center.y - half_height,
		center.x + half_width,
		center.y + half_height);
	compute_projection();
}
void FBGfx::ViewOrthographic::RotateTo(float _z_theta)
{
	rotation_matrix = glm::rotate(
		glm::mat4(1.0),
		_z_theta, 
		glm::vec3(0.0f, 0.0f, 1.0f));
	compute_projection();
}

void FBGfx::ViewOrthographic::RotateTo(
	float _z_theta, 
	const FBGfx::Point _origin)
{
	rotation_matrix = glm::translate(
		glm::rotate(
			glm::translate(
				glm::mat4(1.0),
				glm::vec3(-_origin.x, -_origin.y, 0.0f)),
			_z_theta,
			glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::vec3(_origin.x, _origin.y, 0.0));
	compute_projection();
}
void FBGfx::ViewOrthographic::RotateTo(float _z_theta, float _ox, float _oy)
{
	rotation_matrix = glm::translate(
		glm::rotate(
			glm::translate(
				glm::mat4(1.0),
				glm::vec3(-_ox, -_oy, 0.0f)),
			_z_theta,
			glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::vec3(_ox, _oy, 0.0));
	compute_projection();
}