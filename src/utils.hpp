#pragma once
#include <vector>
#include "glm/glm.hpp"

namespace utils {

float rand(float min, float max);
void  draw_disk(glm::vec2 position, float radius, glm::vec4 const& color);
void  draw_line(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 const& color);
std::vector<glm::vec2> poisson_disc_sampling(glm::vec2 center, float radius, float minDist, int k = 30);

} // namespace utils