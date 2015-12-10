#pragma once
#include "painter.h"

#define VERTEX_DATA_TYPE GLfloat

class Painter::LineSegment2DBatch : public Painter::GraphicsBatch<VERTEX_DATA_TYPE>
{	
public:
	LineSegment2DBatch() : Painter::GraphicsBatch<VERTEX_DATA_TYPE>() {}
	void addLine(GLfloat _x0, GLfloat _y0, GLfloat _x1, GLfloat _y1, GLfloat _z, glm::vec4& _col)
	{
		GLfloat* vDat;
		GLushort* iDat;
		unsigned int vertS;
		vertexData.reserve(14);
		indexData.reserve(2);

		vertS = vertexData.getSize();
		vDat = vertexData.getEndOfData();
		iDat = indexData.getEndOfData();

		vDat[0] = _x0;
		vDat[1] = _y0;
		vDat[2] = _z;
		vDat[3] = _col.r;
		vDat[4] = _col.g;
		vDat[5] = _col.b;
		vDat[6] = _col.a;

		vDat[7] = _x1;
		vDat[8] = _y1;
		vDat[9] = _z;
		vDat[10] = _col.r;
		vDat[11] = _col.g;
		vDat[12] = _col.b;
		vDat[13] = _col.a;

		iDat[0] = vertS;
		iDat[1] = vertS + 1;	
	
		vertexData.acquire(14);
		indexData.acquire(2);
		loadedBuffers = false;
	}
	void addSquare(GLfloat _x0, GLfloat _y0, GLfloat _x1, GLfloat _y1, GLfloat _z, glm::vec4& _col)
	{
		GLfloat* vDat;
		GLushort* iDat;
		unsigned int vertS;
		vertexData.reserve(28);
		indexData.reserve(8);

		vertS = vertexData.getSize();
		vDat = vertexData.getEndOfData();
		iDat = indexData.getEndOfData();

		vDat[0] = _x0;
		vDat[1] = _y0;
		vDat[2] = _z;
		vDat[3] = _col.r;
		vDat[4] = _col.g;
		vDat[5] = _col.b;
		vDat[6] = _col.a;

		vDat[7] = _x0;
		vDat[8] = _y1;
		vDat[9] = _z;
		vDat[10] = _col.r;
		vDat[11] = _col.g;
		vDat[12] = _col.b;
		vDat[13] = _col.a;

		vDat[14] = _x1;
		vDat[15] = _y1;
		vDat[16] = _z;
		vDat[17] = _col.r;
		vDat[18] = _col.g;
		vDat[19] = _col.b;
		vDat[20] = _col.a;

		vDat[21] = _x1;
		vDat[22] = _y0;
		vDat[23] = _z;
		vDat[24] = _col.r;
		vDat[25] = _col.g;
		vDat[26] = _col.b;
		vDat[27] = _col.a;

		iDat[0] = vertS;
		iDat[1] = vertS + 1;
		iDat[2] = vertS + 1;
		iDat[3] = vertS + 2;
		iDat[4] = vertS + 2;
		iDat[5] = vertS + 3;
		iDat[6] = vertS + 3;
		iDat[7] = vertS;

		vertexData.acquire(28);
		indexData.acquire(8);
		loadedBuffers = false;
	}
	
protected:
	void _drawElements()
	{
		glDrawElements(GL_LINES, indexData.getSize(), GL_UNSIGNED_SHORT, nullptr);
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
