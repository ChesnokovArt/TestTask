#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

class Renderer
{
  static GLuint va;
  static GLuint vb;
  static GLuint color_buffer;
  static GLuint vertex_shader;
  static GLuint fragment_shader;
  static GLuint program;
  static GLint vp_location;
  static GLint model_location;
public:
  static void Init();
  static void DrawBox(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = {1.0f, 1.0f, 1.0f}, const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f });
  static void SetVP(const GLfloat* mat) {
    glUniformMatrix4fv(vp_location, 1, GL_FALSE, mat);
  }
};

