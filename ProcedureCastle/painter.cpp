#include "constants.h"
#include "debug.h"
#include "painter.h"
#include "utility.h"
#include <string>
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "batches.h"
#include <float.h>
#include "lodepng.h"
#include <vector>

#define SHADER_PATH RESOURCE_PATH "shaders\\"
#define FONT_PATH RESOURCE_PATH "gfx\\fonts\\"

#define SHADER_SIMPLETRANSFORM_VERT SHADER_PATH "simpletransform.vert"
#define SHADER_SOLIDCOLOR_FRAG SHADER_PATH "solidcolor.frag"

#define FONT_DEBUG FONT_PATH "consolefont.png"

#define IS_SCREENPTR(_X_) (typeid(_X_) == typeid(Painter::ScreenDestination))
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define USE_SHADER_(_X_) if ((Painter::lastUsedProgram != (_X_)) || (!Painter::hasUsedProgram)){ glUseProgram(_X_);
#define _END_USE_SHADER() Painter::hasUsedProgram = true; }

#define PREPARE_BATCH(_X_, _Y_) if (_X_ == nullptr){ _X_ = new _Y_(); _X_->prepareBuffers(); }

#define MAX_Z_ORDER_DRAW 100000

#define BATCH_DEFAULT_SIZE 16
#define ADVANCE_Z_COUNTER -1

const char* Painter::PrimitiveProgram::MVP_UNIFORM_NAME = "u_mvp";

const char* Painter::TextProgram::MVP_UNIFORM_NAME = "u_mvp";
const char* Painter::TextProgram::TEXTURE_UNIFORM_NAME = "u_tex";

GLuint Painter::solidColor_frag;
GLuint Painter::simpleTransform_vert;
Painter::PrimitiveProgram Painter::primitive_prog;
GLuint Painter::lastUsedProgram;
bool Painter::hasUsedProgram = false;
bool Painter::isScreenRendering = true;
bool Painter::hasContext = false;
bool Painter::recordedKeyPress = false;
Painter::LineSegment2DBatch* Painter::primitiveLineBatch = nullptr;
Painter::Triangle2DBatch* Painter::primitivePolyBatch = nullptr;
GLint Painter::curZOrder;
GLuint Painter::debugFont;


const Painter::ScreenDestination Painter::ScreenPointer;
Painter::WindowData_s Painter::windowData;
struct _painterRegistrationObject {
	_painterRegistrationObject() { Painter::staticConstruct(); }
	~_painterRegistrationObject() { Painter::staticDestruct(); }
};
_painterRegistrationObject _painterRegistrationObjectMember;


void Painter::TextProgram::setupAttributes()
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, BUFFER_OFFSET(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, BUFFER_OFFSET(sizeof(GLfloat) * 6));

}
void Painter::TextProgram::enable()
{
	USE_SHADER_(programID)

		Painter::passMat4ToCurrentProgram(mvpMatrix_loc, Painter::windowData.projection2D);
		glUniform1i(texture_loc, 0);

	_END_USE_SHADER()
}


void Painter::PrimitiveProgram::setupAttributes()
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7, BUFFER_OFFSET(sizeof(GLfloat)*3));
}
void Painter::PrimitiveProgram::enable()
{
	USE_SHADER_(programID)

		Painter::passMat4ToCurrentProgram(mvpMatrix_loc, Painter::windowData.projection2D);

	_END_USE_SHADER()
}

template <class T>
Painter::GraphicsBatch<T>::GraphicsBatch()
{
	hasInit = false;
}

template <class T>
Painter::GraphicsBatch<T>::~GraphicsBatch()
{
	if (hasInit)
	{
		glDeleteBuffers(1, &gl_array_index);
		glDeleteBuffers(1, &gl_array_data);
		glDeleteBuffers(1, &gl_array);
	}
}


template <class T>
void Painter::GraphicsBatch<T>::prepareBuffers()
{
	glGenVertexArrays(1, &gl_array);
	glBindVertexArray(gl_array);
	glGenBuffers(1, &gl_array_data);
	glBindBuffer(GL_ARRAY_BUFFER, gl_array_data);
	prepareAttributes();
	glGenBuffers(1, &gl_array_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_array_index);
	loadedBuffers = false;
	hasInit = true;
}

template <class T>
void Painter::GraphicsBatch<T>::drawElements()
{
	glBindVertexArray(gl_array);
	glBindBuffer(GL_ARRAY_BUFFER, gl_array_data);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_array_index);

	if (!loadedBuffers)
	{
		loadedBuffers = true;
		glBufferData(GL_ARRAY_BUFFER, sizeof(T)*vertexData.getSize(), vertexData.getData(), GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indexData.getSize(), indexData.getData(), GL_STREAM_DRAW);
	}
	prepareShaders();
	_drawElements();
}
template <class T>
void Painter::GraphicsBatch<T>::clear()
{
	loadedBuffers = false;
	vertexData.clear();
	indexData.clear();
}

void Painter::resetZOrder()
{
	curZOrder = MAX_Z_ORDER_DRAW;
}

void Painter::staticConstruct()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (!glfwInit()) DEBUG_ERROR("Unable to initialize GLFW");
#else
	glfwInit();
#endif	
	windowData.scrx = -1;
	windowData.scry = -1;
	windowData.windowingMode = INVALID;
	windowData.window = nullptr;
	windowData.title = nullptr;

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
}

void Painter::staticDestruct()
{
	if(hasContext) destructShaders();
	glfwTerminate();
#if DEBUG_SHOULD_LOG
	DEBUG_LOG(INFO, "GLFW Terminated.");
#endif
}

void Painter::constructShaders()
{
#if DEBUG_SHOULD_LOG
	DEBUG_LOG(INFO, "Loading shaders...");
#endif

	simpleTransform_vert = createShader(SHADER_SIMPLETRANSFORM_VERT, GL_VERTEX_SHADER);
	solidColor_frag = createShader(SHADER_SOLIDCOLOR_FRAG, GL_FRAGMENT_SHADER);

	primitive_prog.programID = createShaderProgram(simpleTransform_vert, solidColor_frag);
	primitive_prog.mvpMatrix_loc = glGetUniformLocation(primitive_prog.programID, primitive_prog.MVP_UNIFORM_NAME);

	simpleTransform_vert = createShader(SHADER_SIMPLETRANSFORM_VERT, GL_VERTEX_SHADER);
	solidColor_frag = createShader(SHADER_SOLIDCOLOR_FRAG, GL_FRAGMENT_SHADER);

	primitive_prog.programID = createShaderProgram(simpleTransform_vert, solidColor_frag);
	primitive_prog.mvpMatrix_loc = glGetUniformLocation(primitive_prog.programID, primitive_prog.MVP_UNIFORM_NAME);
}
void Painter::destructShaders()
{
	glDeleteShader(solidColor_frag);
	glDeleteShader(simpleTransform_vert);
	glDeleteProgram(primitive_prog.programID);
}

GLuint Painter::createShader(const char* _filename, GLenum _type)
{
#if DEBUG_SHOULD_ERROR_CHECK
	GLint compileStatus;
	int logLength;
	GLchar* logMsg;
	std::string logOutput;
#endif
	GLuint shaderID;
	std::string codeStr;
	const char * codeCStr;

	codeStr = textFileToString(_filename);
	codeCStr = codeStr.c_str();

	shaderID = glCreateShader(_type);

#if DEBUG_SHOULD_LOG
	logOutput = "Compiling shader: ";
	logOutput.append(_filename);
	DEBUG_LOG(INFO, logOutput.c_str());
#endif
	glShaderSource(shaderID, 1, &codeCStr, NULL);
	glCompileShader(shaderID);

#if DEBUG_SHOULD_ERROR_CHECK
	compileStatus = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus)
	{
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		logMsg = new GLchar[logLength + 1];
		glGetShaderInfoLog(shaderID, logLength, NULL, logMsg);
		logOutput = "Shader Compiler Message (";
		logOutput.append(_filename);
		logOutput.append("):\n");
		logOutput.append(logMsg);
		DEBUG_ERROR(logOutput.c_str());
		delete(logMsg);
	}
#endif
	
	return shaderID;
}

GLuint Painter::createShaderProgram(GLuint _vertShader, GLuint _fragShader)
{
#if DEBUG_SHOULD_ERROR_CHECK
	GLint linkStatus;
	int logLength;
	GLchar* logMsg;
	std::string logOutput;
#endif
	GLuint programID;
	programID = glCreateProgram();
	glAttachShader(programID, _vertShader);
	glAttachShader(programID, _fragShader);
	glLinkProgram(programID);
	
#if DEBUG_SHOULD_ERROR_CHECK
	linkStatus = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
		logMsg = new GLchar[logLength + 1];
		glGetProgramInfoLog(programID, logLength, NULL, logMsg);
		logOutput = "Shader Linker Message:\n";
		logOutput.append(logMsg);
		DEBUG_ERROR(logOutput.c_str());
		delete(logMsg);
	}
#endif

	glDetachShader(programID, _vertShader);
	glDetachShader(programID, _fragShader);

	return programID;
}

GLuint Painter::loadTexture(const char* _filename)
{
	std::vector<unsigned char> texImg;
	GLuint texID;
	unsigned int w, h;

#if DEBUG_SHOULD_ERROR_CHECK
	if (lodepng::decode(texImg, w, h, _filename))
	{
		DEBUG_ERROR("Unable to load texture.");
	}
#else
	lodepng::decode(texImg, w, h, _filename)
#endif

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, &texImg[0]);
}

void Painter::screenConstruct()
{
	resetZOrder();
	hasContext = true;
	glewExperimental = GL_TRUE;
#if DEBUG_SHOULD_ERROR_CHECK
	if (glewInit()) DEBUG_ERROR("Unable to initialize GLEW");
#else
	glewInit();
#endif
	constructShaders();
	//glClearColor(0.3922f, 0.5843f, 0.9294f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glfwSetKeyCallback(windowData.window, keyCallback);
	glEnable(GL_TEXTURE_2D);
	debugFont = loadTexture(FONT_DEBUG);
}
void Painter::screenDestruct()
{
	glDeleteTextures(1, &debugFont);
	glfwDestroyWindow(windowData.window);
	destructShaders();
	if (primitiveLineBatch != nullptr) delete(primitiveLineBatch);
	primitiveLineBatch = nullptr;
	if (primitivePolyBatch != nullptr) delete(primitivePolyBatch);
	primitivePolyBatch = nullptr;
	hasContext = false;
}

void Painter::screen(int _scrw, int _scrh, const char * _title, Mode_e _wmode)
{
	GLFWmonitor* fscreenPtr;
	if (windowData.window != nullptr) screenDestruct();
	
	if (_wmode == FULLSCREEN)
	{
		fscreenPtr = glfwGetPrimaryMonitor();
		windowData.windowingMode = FULLSCREEN;
	}
	else
	{ 
		fscreenPtr = nullptr;
		windowData.windowingMode = WINDOWED;
	}
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	windowData.window = glfwCreateWindow(_scrw, _scrh, _title, fscreenPtr, nullptr);
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not open window.\n");
#endif
	glfwMakeContextCurrent(windowData.window);
	windowData.scrx = _scrw;
	windowData.scry = _scrh;
	if (windowData.title != nullptr) delete(windowData.title);
	windowData.title = new char[strlen(_title) + 1];
	strcpy(windowData.title, _title);
	windowData.projection2D = glm::ortho((float) 0, (float) _scrw, (float) _scrh, (float) 0, FLT_MIN, FLT_MAX);
	glViewport(0, 0, _scrw, _scrh);

	if (!hasContext) screenConstruct();
}


const char* Painter::getWindowTitle()
{
	return windowData.title;
}

void Painter::setWindowTitle(const char* _title)
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not set title because no window is defined.\n");
#endif
	if (windowData.title != nullptr) delete(windowData.title);
	windowData.title = new char[strlen(_title) + 1];
	strcpy(windowData.title, _title);
	glfwSetWindowTitle(windowData.window, _title);
}

Painter::Mode_e Painter::getWindowMode()
{
	return windowData.windowingMode;
}

bool Painter::runApp()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Cannot poll for window button statuses because no window is defined.\n");
#endif
	return !glfwWindowShouldClose(windowData.window);
}

void Painter::cls()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not clear because no window is defined.\n");
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	resetZOrder();
	if (primitiveLineBatch != nullptr) primitiveLineBatch->clear();
	if (primitivePolyBatch != nullptr) primitivePolyBatch->clear();
}

void Painter::flushInput()
{
	recordedKeyPress = false;
}

void Painter::sync()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not sync because no window is defined.\n");
#endif
	if (primitiveLineBatch != nullptr) primitiveLineBatch->drawElements();
	if (primitivePolyBatch != nullptr) primitivePolyBatch->drawElements();
	glfwSwapBuffers(windowData.window);
	flushInput();
	glfwPollEvents();
}

bool Painter::keyPressed()
{
	return recordedKeyPress;
}

void Painter::keyCallback(GLFWwindow* _window, GLint _key, GLint _scancode, GLint _action, GLint _mods)
{
	if (_action = GLFW_PRESS)
	{
		recordedKeyPress = true;
	}
}

void Painter::waitForKeyPress()
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not poll events because no window is defined.\n");
#endif
	flushInput();
	while (!keyPressed()) glfwPollEvents();
}

void Painter::setRenderTarget( /* something here eventually */)
{
	renderToScreen();
}
void Painter::renderToScreen()
{
	if (!isScreenRendering)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowData.scrx, windowData.scry);
		isScreenRendering = true;
	}	
}
void Painter::passMat4ToCurrentProgram(GLuint _loc, glm::mat4& _mat4)
{
	glUniformMatrix4fv(_loc, 1, GL_FALSE, glm::value_ptr(_mat4));
}
void Painter::passVec4ToCurrentProgram(GLuint _loc, glm::vec4& _vec4)
{
	glUniform4fv(_loc, 1, glm::value_ptr(_vec4));
}

void Painter::line
	(
		GLfloat _x0, GLfloat _y0,
		GLfloat _x1, GLfloat _y1,
		Color_t _col,
		Mode_e _mode
	)
{
	line(ScreenPtr, _x0, _y0, _x1, _y1, _col, _mode);
}

// runs in about 0.0162ms, pretty slow, but this is expected
void Painter::line
	(
		RenderDestination _dest,
		GLfloat _x0, GLfloat _y0,
		GLfloat _x1, GLfloat _y1,
		Color_t _col,
		Mode_e _mode
	)
{
#if DEBUG_SHOULD_ERROR_CHECK
	if (windowData.window == nullptr) DEBUG_ERROR("Could not draw line because no window is defined.\n");
#endif
	
	switch (_mode)
	{
	case LINE:
		PREPARE_BATCH(primitiveLineBatch, LineSegment2DBatch);
		primitiveLineBatch->addLine(_x0, _y0, _x1, _y1, curZOrder, _col);
		break;
	case B:
		PREPARE_BATCH(primitiveLineBatch, LineSegment2DBatch);
		primitiveLineBatch->addSquare(_x0, _y0, _x1, _y1, curZOrder, _col);
		break;
	case BF:
		PREPARE_BATCH(primitivePolyBatch, Triangle2DBatch);
		primitivePolyBatch->addBox(_x0, _y0, _x1, _y1, curZOrder, _col);
		break;
	}
	
	curZOrder += ADVANCE_Z_COUNTER;
}

