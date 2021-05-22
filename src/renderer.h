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
  GLint view_location;
  GLint proj_location;
  GLint model_location;

  GLuint fsQuad;
  GLuint pass_location;
  GLuint weight_location;
  GLuint renderFBO, intermediateFBO;
  GLuint renderTex, intermediateTex;

  GLuint focal_dist_loc;
  GLuint focal_range_loc;
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
  static void SetView(const GLfloat* mat) {
    glUniformMatrix4fv(sd.view_location, 1, GL_FALSE, mat);
  }
  static void SetProjection(const GLfloat* mat) {
    glUniformMatrix4fv(sd.proj_location, 1, GL_FALSE, mat);
  }
  static void SetFocalDistance(float dist) {
    glUniform1f(sd.focal_dist_loc, dist);
  }
  static void SetFocalRange(float range) {
    glUniform1f(sd.focal_range_loc, range);
  }

private:
  static float gauss(float, float);
};

