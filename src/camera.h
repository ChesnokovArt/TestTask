#pragma once

#include <utility>
#include <glm/glm.hpp>

class Camera {
public:
  Camera() = default;
  //Camera(const glm::mat4& proj) : m_ProjectionMatrix(proj) {}
  Camera(float fov, float aspect, float near, float far);

  void OnUpdate(double timeStep);

  void SetViewportSize(float width, float height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    UpdateProjection();
  }

  const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
  glm::mat4 GetViewProjection() const {
    return m_ProjectionMatrix * m_ViewMatrix;
  }
  const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

  glm::vec3 GetUpDirection() const;
  glm::vec3 GetRightDirection() const;
  glm::vec3 GetForwardDirection() const;
  const glm::vec3& GetPosition() const { return m_Position; }
  glm::quat GetOrientation() const;

  float GetPitch() const { return m_Pitch; }
  float GetYaw() const { return m_Yaw; }

  virtual ~Camera() = default;
private:
  void UpdateProjection();
  void UpdateView();

  void MousePan(const glm::vec2& delta);
  void MouseRotate(const glm::vec2& delta);
  void MouseZoom(float delta);

  glm::vec3 CalculatePosition() const;

  std::pair<float, float> PanSpeed() const;
  float RotationSpeed() const;
  float ZoomSpeed() const;
protected:
  float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

  glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
  glm::mat4 m_ViewMatrix;
  glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
  glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

  glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

  float m_Distance = 10.0f;
  float m_Pitch = 0.0f, m_Yaw = 0.0f;

  float m_ViewportWidth = 1280, m_ViewportHeight = 720;

};