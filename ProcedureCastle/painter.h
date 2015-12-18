#pragma once
#include "glew.h"
#include "glfw3.h"
#include "glm\mat4x4.hpp"
#include "glm\vec4.hpp"
#include "glm\vec3.hpp"
#include "darray.h"

// needs threading support

#define ScreenPtr Painter::ScreenPointer

#define _PAINTER_VERSION_STR_ "Painter 1.0b"

class Painter
{
	friend struct _painterRegistrationObject;
public:
	typedef glm::fvec4 Color_t;
	class RenderDestination
	{
	public:
		RenderDestination() {};
		RenderDestination(GLuint _destInt) { destInt = _destInt; }
		template<typename T>
		RenderDestination& operator=(T const &value) { destInt = value; return *this }
		operator GLuint() { return destInt; }
	private:
		GLuint destInt;
	};
	enum Mode_e
	{
		INVALID,
		FULLSCREEN,
		WINDOWED,
		LINE,
		B,
		BF
	};
private:
	template <class T>
	class GraphicsBatch
	{
	public:
		GraphicsBatch();
		~GraphicsBatch();
		void drawElements();
		void clear();
		void prepareBuffers();
	protected:
		virtual void _drawElements() = 0;
		virtual void prepareAttributes() = 0;
		virtual void prepareShaders() = 0;
		GLuint gl_array;
		GLuint gl_array_data;
		GLuint gl_array_index;
		bool loadedBuffers;
		bool hasInit;
		DArray<GLfloat> vertexData;
		DArray<GLushort> indexData;
	};
	class LineSegment2DBatch;
	class Triangle2DBatch;
	class Text2DBatch;
	class ScreenDestination : public RenderDestination {};
	struct WindowData_s
	{
		int scrx;
		int scry;
		Mode_e windowingMode;
		GLFWwindow* window;
		char* title;
		glm::mat4 projection2D;
	};

public:
	static const ScreenDestination ScreenPointer;
	static void screen(int _scrw, int _scrh, const char* _title = _PAINTER_VERSION_STR_, Mode_e _wmode = WINDOWED);
	static const char* getWindowTitle();
	static void setWindowTitle(const char* _title = _PAINTER_VERSION_STR_);
	static Mode_e getWindowMode();
	static void sync();
	static void cls();
	static bool runApp();
	static void waitForKeyPress();
	static bool keyPressed();
	static void flushInput();
	

	// 2D Line Drawing: render to target
	static void line
		(
			RenderDestination _dest,
			GLfloat _x0, GLfloat _y0,
			GLfloat _x1, GLfloat _y1,
			Color_t _col = Color_t(1.0, 1.0, 1.0, 1.0),
			Mode_e _mode = LINE
		);

	// 2D Line Drawing: render to screen
	static void line
		(
			GLfloat _x0, GLfloat _y0,
			GLfloat _x1, GLfloat _y1,
			Color_t _col = Color_t(1.0, 1.0, 1.0, 1.0),
			Mode_e _mode = LINE
		);

private:
	static void staticConstruct();
	static void staticDestruct();
	static void constructShaders();
	static void destructShaders();
	static void screenConstruct();
	static void screenDestruct();

	static void resetZOrder();
	static GLuint createShader(const char* _filename, GLenum _type);
	static GLuint createShaderProgram(GLuint _vertShader, GLuint _fragShader);
	static void passMat4ToCurrentProgram(GLuint _loc, glm::mat4& _mat4);
	static void passVec4ToCurrentProgram(GLuint _loc, glm::vec4& _vec4);

	static GLuint loadTexture(const char* _filename);


	static void keyCallback(GLFWwindow* _window, GLint _key, GLint _scancode, GLint _action, GLint _mods);
	static bool recordedKeyPress;

	static void setRenderTarget(/* something here eventually */);
	static void renderToScreen();

	static WindowData_s windowData;
	static GLuint debugFont;

	static GLint curZOrder;

	static LineSegment2DBatch* primitiveLineBatch;
	static Triangle2DBatch* primitivePolyBatch;

	static ShaderProgram primitive_prog;

	static GLuint lastUsedProgram;
	static bool hasUsedProgram;
	static bool isScreenRendering;
	static bool hasContext;
};


