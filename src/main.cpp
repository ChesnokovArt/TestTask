#include <iostream>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "input.h"
#include "camera.h"

static const struct
{
  float x, y;
  float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"in vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

void error_callback(int error, const char* description);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  std::cout << "HelloWorld!" << std::endl;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow* window = glfwCreateWindow(1280, 720, "TestTask", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -2;
  }

  Input::Init(window);

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint mvp_location = glGetUniformLocation(program, "MVP");
  GLint vpos_location = glGetAttribLocation(program, "vPos");
  GLint vcol_location = glGetAttribLocation(program, "vCol");

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
    sizeof(vertices[0]), (void*)0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    sizeof(vertices[0]), (void*)(sizeof(float) * 2));

  Camera cam;

  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    glm::mat4 m, p, mvp;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    cam.SetViewportSize(width, height);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    /*m = glm::rotate(glm::identity<glm::mat4>(), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, -1.0f));
    //mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mvp = m * p;*/

    mvp = cam.GetViewProjection();
    cam.OnUpdate(1 / 60.0);

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);


    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}