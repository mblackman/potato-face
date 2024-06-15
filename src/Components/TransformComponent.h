#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec2 position;
	double rotation;
	glm::vec2 scale;
};