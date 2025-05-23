#include <iostream>
#include <cstdlib>
#include <ctime>
#include "utils.hpp"
#include <glm/glm.hpp>

struct Particle {
    const float min = -1.0f; // Minimum position value
    const float max = 1.0f;  // Maximum position value

    glm::vec2 position; // Particle position

    // Constructor to initialize the position randomly
    Particle() {}

    // Function to display the particle's position
    void Display() const {
        std::cout << "Particle Position: (" << position.x << ", " << position.y << ")\n";
    }
};