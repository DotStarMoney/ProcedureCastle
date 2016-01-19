#include "fbgfx.h"

#define CLEAR_COLOR 0.1373, 0.1373, 0.1373, 1.0


void FBGfx::staticConstruct()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (!glfwInit()) DEBUG_ERROR("Unable to initialize GLFW");
#else
	glfwInit();
#endif	
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	has_context = false;
	recorded_key_press = false;
}

void FBGfx::staticDestruct()
{
	if (has_context) glfwDestroyWindow(window.window);
	// fill in
}

void FBGfx::Screen(
	int _scrw,
	int _scrh,
	const char* _title = _FBGFX_VERSION_STR_,
	WindowingMode _wmode = WindowingMode::WINDOWED)
{
	GLFWmonitor* fscreen_ptr;
	GLFWwindow* new_window;

	if (_wmode == WindowingMode::FULLSCREEN)
	{
		fscreen_ptr = glfwGetPrimaryMonitor();
		window.windowing_mode = WindowingMode::FULLSCREEN;
	}
	else
	{
		fscreen_ptr = nullptr;
		window.windowing_mode = WindowingMode::WINDOWED;
	}

	if (!has_context)
	{
		window.window = glfwCreateWindow(
				_scrw, 
				_scrh, 
				_title, 
				fscreen_ptr, 
				NULL);
		glfwMakeContextCurrent(window.window);
		window.title = std::string(_title);
		window.scrx = _scrw;
		window.scry = _scrh;
		on_window_size_update();
		on_context_first_create();
		on_new_window();
	}
	else
	{
		if (window.windowing_mode != _wmode)
		{
			new_window = glfwCreateWindow(
					_scrw,
					_scrh,
					_title,
					fscreen_ptr,
					window.window);
			glfwDestroyWindow(window.window);
			window.window = new_window;
			glfwMakeContextCurrent(window.window);
			glfwSwapInterval(1);
			window.title = std::string(_title);
			window.scrx = _scrw;
			window.scry = _scrh;
			on_new_window();
			on_window_size_update();
		}
		else
		{
			if ((_scrw != window.scrx) || (_scrh != window.scry))
			{
				glfwSetWindowSize(window.window, _scrw, _scrh);
				on_window_size_update();
			}
			if (strcmp(window.title.c_str(), _title))
			{
				window.title = std::string(_title);
			}
		}
	}
}
void FBGfx::on_new_window()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glfwSetKeyCallback(window.window, glfw_key_callback);
	glEnable(GL_TEXTURE_2D);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glClearColor(CLEAR_COLOR);
}

const char* FBGfx::get_window_title()
{
	return window.title.c_str();
}
void FBGfx::set_window_title(const char* _title = _FBGFX_VERSION_STR_)
{
	window.title = std::string(_title);
}
FBGfx::WindowingMode FBGfx::get_window_mode()
{
	return window.windowing_mode;
}

void FBGfx::on_window_size_update()
{
	// fill in
	//    need to update view matrices and what not
	//    set viewport of current view
}
void FBGfx::on_context_first_create()
{
	has_context = true;
	glewExperimental = GL_TRUE;
#if DEBUG_SHOULD_ERROR_CHECK
	if (glewInit()) DEBUG_ERROR("Unable to initialize GLEW");
#else
	glewInit();
#endif
	// fill in
}

void FBGfx::Sync()
{
	// fill in
}
void FBGfx::Cls(Color clear_color)
{
	glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	Cls();
}
void FBGfx::Cls()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// fill in
}

void FBGfx::glfw_key_callback(
	GLFWwindow* _window,
	GLint _key,
	GLint _scancode,
	GLint _action,
	GLint _mods)
{
	if (_action = GLFW_PRESS)
	{
		recorded_key_press = true;
	}
	// fill in
}

void FBGfx::WaitForKeyPress()
{
	FlushInput();
	while (!KeyPressed()) glfwPollEvents();
}
bool FBGfx::KeyPressed()
{
	return recorded_key_press;
}
void FBGfx::FlushInput()
{
	recorded_key_press = false;
}