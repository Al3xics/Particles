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
        float speed = utils::rand(0.1f, 0.2f);

        velocity = speed * glm::vec2(std::cos(angle), std::sin(angle)); // initial speed
    
        mass = utils::rand(0.1f, 1.0f);
        lifetime = utils::rand(15.0f, 20.0f); // Particle lives between 15 and 20 seconds
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
        float bounce = std::abs(std::sin(10.f * fade * glm::pi<float>()) * (1.f - fade));
        return startRadius * (1.0f - fade) + 0.005f * bounce;
    }

    glm::vec4 color() const {
        float p = 3.f;
        float t = glm::clamp(age / lifetime, 0.f, 1.f);
        float left = std::pow(glm::min(2.f * t, 1.f), p);
        float right = std::pow(glm::min(2.f * (1.f - t), 1.f), p);
        float easedT = 0.5f * (left + (2.f - right));
        return (1.f - easedT) * startColor + easedT * endColor;
    }

    // Function to display the particle's position
    void display() const {
        std::cout << "Particle Position: (" << position.x << ", " << position.y << ")\n";
    }
};