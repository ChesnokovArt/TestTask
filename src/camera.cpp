#include "camera.h"
#include "input.h"
#include <glm/ext.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float fov, float aspect, float near, float far)
  : m_FOV(fov), m_AspectRatio(aspect), m_NearClip(near), m_FarClip(far),
    m_ProjectionMatrix(glm::perspective(glm::radians(fov), aspect, near, far))
{
	UpdateView();
}

void Camera::UpdateProjection()
{
	m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
	m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

void Camera::UpdateView()
{
	m_Position = CalculatePosition();

	glm::quat orientation = GetOrientation();
	m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
	m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

std::pair<float, float> Camera::PanSpeed() const
{
	float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
	float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

	float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
	float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

	return { xFactor, yFactor };
}

float Camera::RotationSpeed() const
{
	return 0.8f;
}

float Camera::ZoomSpeed() const
{
	float distance = m_Distance * 0.2f;
	distance = std::max(distance, 0.0f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f); // max speed = 100
	return speed;
}

void Camera::OnUpdate(double ts)
{
	if (Input::IsKeyPressed(Key::LeftAlt))
	{
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			MousePan(delta);
		else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
			MouseRotate(delta);
		else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
			MouseZoom(delta.y);
	}

	UpdateView();
}

void Camera::MousePan(const glm::vec2& delta)
{
	auto [xSpeed, ySpeed] = PanSpeed();
	m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
	m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
}

void Camera::MouseRotate(const glm::vec2& delta)
{
	float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
	m_Yaw += yawSign * delta.x * RotationSpeed();
	m_Pitch += delta.y * RotationSpeed();
}

void Camera::MouseZoom(float delta)
{
	m_Distance -= delta * ZoomSpeed();
	if (m_Distance < 1.0f)
	{
		m_FocalPoint += GetForwardDirection();
		m_Distance = 1.0f;
	}
}

glm::vec3 Camera::GetUpDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::GetRightDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::GetForwardDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Camera::CalculatePosition() const
{
	return m_FocalPoint - GetForwardDirection() * m_Distance;
}

glm::quat Camera::GetOrientation() const
{
	return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}
