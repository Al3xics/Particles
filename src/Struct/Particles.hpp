#include <iostream>
#include <cstdlib>
#include <ctime>
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// -----------------------------
// Physical constants & settings
// -----------------------------
const glm::vec2 GRAVITY = glm::vec2(0.0f, -2.f); // Gravity of the earth : -9.81f
constexpr float DRAG_COEFF = 1.5f;  // Air‑drag coefficient (linear, low‑speed regime)
constexpr float SPRING_K = 5.0f;  // Spring stiffness to the mouse position
constexpr float VORTEX_STRENGTH = 2.0f;  // Strength of the vortex (tangential) force
const glm::vec2 VORTEX_CENTER(0.0f, 0.0f); // Point around which particles rotate


struct Particle {
    glm::vec2 position; // Particle position
    glm::vec2 velocity; // Particle velocity
    float mass; // Particle mass

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
    }

    // Full Euler integration of all the forces
    void update(float dt, const glm::vec2& mousePos) {
        // 1. Gravity (weight)
        glm::vec2 gravity = GRAVITY * mass; // F = m g

        // 2. Linear air‑drag (low‑speed)
        glm::vec2 drag = -DRAG_COEFF * velocity;  // F = -γ v

        // 3. Spring towards the mouse: F = k (x_target − x)
        glm::vec2 spring = SPRING_K * (mousePos - position);

        // 4. Vortex force : tangential to radius w.r.t. a center
        glm::vec2 r = position - VORTEX_CENTER;
        glm::vec2 perp(-r.y, r.x); // (−y, x) gives a 90° CW rotation
        glm::vec2 vortex = VORTEX_STRENGTH * perp;

        // Resultant force & Newton’s 2nd law
        glm::vec2 totalForce = gravity + drag + spring + vortex;
        glm::vec2 accel = totalForce / mass; // a = ΣF / m

        // Semi‑implicit Euler : v ← v + a·dt, x ← x + v·dt
        velocity += accel * dt;
        position += velocity * dt;
    }

    // Function to display the particle's position
    void Display() const {
        std::cout << "Particle Position: (" << position.x << ", " << position.y << ")\n";
    }
};