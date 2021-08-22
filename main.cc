#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

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

  if (glewInit() != GLEW_OK) {
    cout << "Error ! " << endl;
  }
  cout << glGetString(GL_VERSION) << endl;

  float positions[6] = {
    -0.5f, -0.5f,
    0.0f,  0.5f,
    0.5f, -0.5f,
  };
  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

  ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
  string vertexShader = source.VertexSource;
  string fragmentShader = source.FragmentSource;
  unsigned int shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}