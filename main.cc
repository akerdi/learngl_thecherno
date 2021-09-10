#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#define ASSERT(x) if (!(x)) __builtin_trap();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
  while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
  while (GLenum error = glGetError()) {
    cout << "[OpenGL Error] (" << error << "): " << function << " " << file << " : " << line << endl;
    return false;
  }
  return true;
}

enum class ShaderType {
  NONE = -1, VERTEX = 0, FRAGMENT = 1
};
struct ShaderProgramSource {
  string VertexSource;
  string FragmentSource;
};

static ShaderProgramSource ParseShader(const string& filepath) {
  ifstream stream(filepath);

  string line;
  stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line)) {
    if (line.find("#shader") != string::npos) {
      if (line.find("vertex") != string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != string::npos) {
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << '\n';
    }
  }
  return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const string& source) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)alloca(length*sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) << " shader! " << endl;
    cout << message << endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}

static int CreateShader(const string& vertextShader, string& fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertextShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

int main(void) {
  GLFWwindow *window;
  if (!glfwInit()) {
    cout << "glfw init err" << endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  cout << "I'm apple machine" << endl;
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  unsigned int width = 800;
  unsigned int height = 600;
  window = glfwCreateWindow(width, height, "Learngl", NULL, NULL);
  if (!window) {
    cout << " can't create window!!!" << endl;
    glfwTerminate();
    return -1;
  }
  unsigned int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  unsigned int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  cout << "oepngl shader version: " << major << "." << minor << endl;
  glfwMakeContextCurrent(window);

  glfwSwapInterval(500);

  if (glewInit() != GLEW_OK) {
    cout << "Error ! " << endl;
  }
  cout << glGetString(GL_VERSION) << endl;

  float positions[] = {
    -0.5f, -0.5f,
    0.5f,  -0.5f,
    0.5f, 0.5f,
    -0.5f,  0.5f,
  };

  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int buffer;
  GLCall(glGenBuffers(1, &buffer));
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), positions, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

  unsigned int ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *sizeof(unsigned int), indices, GL_STATIC_DRAW))

  ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
  string vertexShader = source.VertexSource;
  string fragmentShader = source.FragmentSource;
  unsigned int shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);

  int location = glGetUniformLocation(shader, "u_Color");
  ASSERT(location != -1);

  glBindVertexArray(0);
  glUseProgram(0);
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  float r = 0.f, g = 0.f, b = 0.f;
  float incrementr = 0.f, incrementg = 0.f, incrementb = 0.f;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    GLCall(glUseProgram(shader))
    GLCall(glUniform4f(location, r, g, b, 1.0f))
    GLCall(glBindVertexArray(vao))
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo))

    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr))

    if (r >= 1.0f)
      incrementr = -0.02f;
    else if (r <= 0.f)
      incrementr = 0.02f;
    if (g >= 1.f)
      incrementg = -0.01f;
    else if (g <= 0.f)
      incrementg = 0.01f;
    if (b >= 1.f)
      incrementb = -0.05;
    else if (b <= 0.0f)
      incrementb = 0.05;
    r += incrementr;
    g += incrementg;
    b += incrementb;

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &buffer);
  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}