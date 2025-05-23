#include <iostream>
#include <cstdlib>
#include <ctime>
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct Particle {
    glm::vec2 position; // Particle position
    glm::vec2 velocity; // Particle velocity
    float mass; // Particle mass
    float lifetime; // Total lifespan
    float age; // Current age

    // Constructor to initialize the position randomly
    Particle() {
        position = glm::vec2(
            -gl::window_aspect_ratio() + static_cast<float>(std::rand()) / RAND_MAX * 2 * gl::window_aspect_ratio(),
            -1.0f + static_cast<float>(std::rand()) / RAND_MAX * 2.0f
        );

        // Generate random velocity and angle
        float angle = utils::rand(0.f, glm::two_pi<float>()); // 0 à 2π
        float speed = utils::rand(0.01f, 0.1f);

        velocity = speed * glm::vec2(std::cos(angle), std::sin(angle)); // initial speed
    
        mass = utils::rand(0.1f, 1.0f);
        lifetime = utils::rand(2.0f, 5.0f); // Particle lives between 2 and 5 seconds
        age = 0.0f;
    }

    // Full Euler integration of all the forces
    void update(float dt) {
        position += velocity * dt;

        // Update age
        age += dt;
    }

    // Check if the particle has expired
    bool isDead() const {
        return age >= lifetime;
    }

    // Function to display the particle's position
    void display() const {
        std::cout << "Particle Position: (" << position.x << ", " << position.y << ")\n";
    }
};