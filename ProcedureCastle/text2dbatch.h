#pragma once
#pragma once
#include "painter.h"

#define VERTEX_DATA_TYPE GLfloat

class Painter::Text2DBatch : public Painter::GraphicsBatch<VERTEX_DATA_TYPE>
{
public:
	Text2DBatch() : Painter::GraphicsBatch<VERTEX_DATA_TYPE>() {}
	

protected:
	void _drawElements()
	{
		glDrawElements(GL_TRIANGLES, indexData.getSize(), GL_UNSIGNED_SHORT, nullptr);
	}
	void prepareShaders()
	{
		Painter::primitive_prog.enable();
	}
	void prepareAttributes()
	{
		Painter::primitive_prog.setupAttributes();
	}

};
