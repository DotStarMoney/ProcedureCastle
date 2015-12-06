#pragma once
#include "glew.h"
#include "glfw3.h"
#include "glm\mat4x4.hpp"
#include "glm\vec4.hpp"
#include "glm\vec3.hpp"

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
	struct PrimitiveProgram_s
	{
		static const char* MVP_UNIFORM_NAME;
		static const char* COLOR_UNIFORM_NAME;
		GLuint programID;
		GLint mvpMatrix_loc;
		GLint color_loc;
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

	static GLuint createShader(const char* _filename, GLenum _type);
	static GLuint createShaderProgram(GLuint _vertShader, GLuint _fragShader);
	static void passMat4ToCurrentProgram(GLuint _loc, glm::mat4& _mat4);
	static void passVec4ToCurrentProgram(GLuint _loc, glm::vec4& _vec4);

	static void keyCallback(GLFWwindow* _window, GLint _key, GLint _scancode, GLint _action, GLint _mods);
	static bool recordedKeyPress;

	static void setRenderTarget(/* something here eventually */);
	static void renderToScreen();

	static WindowData_s windowData;

	static GLuint solidColor_frag;
	static GLuint simpleTransform_vert;
	static PrimitiveProgram_s primitive_prog;
	
	static GLuint lastUsedProgram;
	static bool hasUsedProgram;
	static bool isScreenRendering;
	static bool hasContext;
};

