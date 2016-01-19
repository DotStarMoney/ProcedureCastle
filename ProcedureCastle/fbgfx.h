#ifndef FBGFX_H
#define FBGFX_H

#include "glew.h"
#include "glfw3.h"
#include "glm\mat4x4.hpp"
#include "glm\vec4.hpp"
#include "glm\vec2.hpp"
#include <string>
#include <vector>
#include "debug.h"


#define _FBGFX_VERSION_STR_ "FBGfx 1.0b"

class FBGfx
{
	// --------------- Do we need all of these? -----------------
	friend class StaticInitializationObject;
	friend class RenderTarget;
	friend class FrameBuffer;
	friend class Texture;
	friend class RenderBuffer;
	friend class Font;
	friend class Shader;
	friend class View;
	// ----------------------------------------------------------
public:
	typedef glm::fvec4 Color;
	typedef glm::fvec2 Point;
	typedef GLuint UInt;
	typedef GLfloat Float;
	struct Quad
	{
		Quad() {}
		Quad(Float _x0, Float _y0, Float _x1, Float _y1)
		{
			x0 = _x0;
			y0 = _y0;
			x1 = _x1;
			y1 = _y1;
		}
		Quad(Point& _tl, Point& _br)
		{
			x0 = _tl.x;
			y0 = _tl.y;
			x1 = _br.x;
			y1 = _br.y;
		}
		Float x0;
		Float y0;
		Float x1;
		Float y1;
	};
	enum class PrimitiveMode
	{
		LINE,
		B,
		BF
	};
	enum class WindowingMode
	{
		FULLSCREEN,
		WINDOWED
	};

	enum class PolyLineMode
	{
		OPEN,
		CLOSED
	};

	enum class PutMode
	{
		PSET,
		TRANS,
		ALPHA
	};

	enum class GraphicsDirective
	{
		SCALE_MODE,
		BLEND_MODE,
	};
	
	enum class GraphicsDirectiveValue
	{
		SCALE_NEAREST_NEIGHBOR,
		SCALE_LINEAR,
		BLEND_ALPHA,
		BLEND_ORDER_INDEPENDANT
	};

	class RenderTarget;
	class FrameBuffer;
	class Texture;
	class RenderBuffer;

	class Font;
	
	class Shader;
	
	class View;
	class ViewOrthographic;
	class ViewPerspective;
	
private:
	class GraphicsBatch;
	class LineSegment2DBatch;
	class ColorTriangle2DBatch;
	class SimpleSprite2DBatch;

	struct WindowData
	{
		int scrx;
		int scry;
		WindowingMode windowing_mode;
		GLFWwindow* window;
		std::string title;
	};

	class StaticInitializationObject
	{
	public:
		StaticInitializationObject() { FBGfx::staticConstruct(); }
		~StaticInitializationObject() { FBGfx::staticDestruct(); }
	};
	FBGfx();
public:
	FBGfx(const FBGfx&) = delete;
	FBGfx& operator=(const FBGfx&) = delete;
	static void Screen(
			int _scrw, 
			int _scrh, 
			const char* _title = _FBGFX_VERSION_STR_, 
			WindowingMode _wmode = WindowingMode::WINDOWED);
	static const char* get_window_title();
	static void set_window_title(const char* _title = _FBGFX_VERSION_STR_);
	static WindowingMode get_window_mode();
	static void Sync();
	static void Cls(Color clear_color);
	static void Cls();
	static void WaitForKeyPress();
	static bool KeyPressed();
	static void FlushInput();

	static void SetTarget(RenderTarget& _target);
	static void UseShader(Shader& _shader);
	static void SetView(View& _view);
	static void SetDirective(
			GraphicsDirective& _directive,
			GraphicsDirectiveValue& _value);

	static const RenderBuffer& ScreenPtr();
	static const Font& DefaultFont();
	static const View& DefaultView();

	static void Line(
			RenderTarget& _target,
			Point& _tl,
			Point& _br,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			PrimitiveMode _mode = PrimitiveMode::LINE);
	static void Line(
			Point& _tl,
			Point& _br,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			PrimitiveMode _mode = PrimitiveMode::LINE);

	static void Polygon(
			std::vector<Point>& _points,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0));
	static void Polygon(
			RenderTarget& _target,
			std::vector<Point>& _points,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0));

	static void PolyLine(
			std::vector<Point>& _points,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			bool _is_closed = false);
	static void PolyLine(
			RenderTarget& _target,
			std::vector<Point>& _points,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			bool _is_closed = false);

	static void Circle(
			RenderTarget& _target,
			Point& _center,
			Float _radius,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			PrimitiveMode _mode = PrimitiveMode::LINE);
	static void Circle(
			Point& _center,
			Float _radius,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			PrimitiveMode _mode = PrimitiveMode::LINE);

	static void Put(
			RenderTarget& _target,
			Point& _tl,
			Texture& _source,
			PutMode _mode = PutMode::PSET);
	static void Put(
			Point& _tl,
			Texture& _source,
			PutMode _mode = PutMode::PSET);
	static void Put(
			RenderTarget& _target,
			Point& _tl,
			Texture& _source,
			Quad& _src_quad,
			PutMode _mode = PutMode::PSET);
	static void Put(
			Point& _tl,
			Texture& _source,
			Quad& _src_quad,
			PutMode _mode = PutMode::PSET);
	static void Put(
			RenderTarget& _target,
			Point& _tl,
			Texture& _source,
			Float _scaleX,
			Float _scaleY,
			Float _angle,
			PutMode _mode = PutMode::PSET);
	static void Put(
			Point& _tl,
			Texture& _source,
			Float _scaleX,
			Float _scaleY,
			Float _angle,
			PutMode _mode = PutMode::PSET);
	static void Put(
			RenderTarget& _target,
			Point& _tl,
			Texture& _source,
			Quad& _src_quad,
			Float _scaleX,
			Float _scaleY,
			Float _angle,
			PutMode _mode = PutMode::PSET);
	static void Put(
			Point& _tl,
			Texture& _source,
			Quad& _src_quad,
			Float _scaleX,
			Float _scaleY,
			Float _angle,
			PutMode _mode = PutMode::PSET);

	static void Text(
			RenderTarget& _target,
			Point& _tl,
			const char* _text,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			Font& _font = DefaultFont);
	static void Text(
			RenderTarget& _target,
			Point& _tl,
			const char* _text,
			Color& _col = Color(1.0, 1.0, 1.0, 1.0),
			Font& _font = DefaultFont);

	static void Blit(
			RenderTarget& _dest,
			Quad& _dest_quad,
			RenderTarget& _src,
			Quad& _src_quad);

private:
	static StaticInitializationObject initializer;
	static View default_view;
	static Font default_font;
	static RenderBuffer screen_ptr;
	static WindowData window;

	static bool has_context;
	static void staticConstruct();
	static void staticDestruct();

	static void on_window_size_update();
	static void on_context_first_create();
	static void on_new_window();

	static bool recorded_key_press;
	static void glfw_key_callback(
			GLFWwindow* _window,
			GLint _key,
			GLint _scancode,
			GLint _action,
			GLint _mods);

	/*
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

	static void setRenderTarget( something here eventually );
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
	*/
};


#endif