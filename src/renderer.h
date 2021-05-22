#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

struct RenderData {
public:
  GLuint va;
  GLuint vb;
  GLuint color_buffer;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLuint program;
  GLint vp_location;
  GLint model_location;

  GLuint fsQuad;
  GLuint pass_location;
  GLuint weight_location;
  GLuint renderFBO, intermediateFBO;
  GLuint renderTex, intermediateTex;
};

class Renderer
{
  Renderer() {};
  static RenderData sd;
public:
  static void Init();
  static void StartDOF();
  static void EndDOF();
  static void DrawBox(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = {1.0f, 1.0f, 1.0f},
    const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f });
  static void SetVP(const GLfloat* mat) {
    glUniformMatrix4fv(sd.vp_location, 1, GL_FALSE, mat);
  }

private:
  static float gauss(float, float);
};

