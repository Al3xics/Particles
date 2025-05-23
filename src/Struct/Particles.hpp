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
    float startRadius; // Initial render radius
    glm::vec4 startColor; // couleur initiale
    glm::vec4 endColor; // couleur finale

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
        startRadius = 0.02f; // same size as before
        auto rand_color = []() {
            return glm::vec4(
                utils::rand(0.f, 1.f),  // Red
                utils::rand(0.f, 1.f),  // Green
                utils::rand(0.f, 1.f),  // Blue
                1.0f                    // Alpha opaque au départ
            );
        };

        startColor = rand_color();
        endColor = rand_color();
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

    // Radius that linearly shrinks to 0 at end of life
    float radius() const {
        float fade = std::clamp((2.0f - (lifetime - age)) / 2.0f, 0.f, 1.f); // 0 si plus de 2 sec restantes, 1 si mort imminente
        return startRadius * (1.0f - fade);
    }

    glm::vec4 color() const {
        float t = std::clamp(age / lifetime, 0.f, 1.f); // progression de vie [0,1]
        return (1.f - t) * startColor + t * endColor; // interpolation linéaire
    }

    // Function to display the particle's position
    void display() const {
        std::cout << "Particle Position: (" << position.x << ", " << position.y << ")\n";
    }
};