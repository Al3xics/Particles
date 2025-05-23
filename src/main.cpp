#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include "Struct/Particles.hpp"
#include <vector>
#include <cstdlib> // Pour std::rand et std::srand
#include <ctime>   // Pour std::time

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Créer un tableau de particules
    std::vector<Particle> particles(100);
    for (auto& particle : particles)
    {
        particle.position = glm::vec2(
            -gl::window_aspect_ratio() + static_cast<float>(std::rand()) / RAND_MAX * 2 * gl::window_aspect_ratio(),
            -1.0f + static_cast<float>(std::rand()) / RAND_MAX * 2.0f
        );
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Afficher les particules
        for (const auto& particle : particles)
        {
            utils::draw_disk(particle.position, 0.01f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }
}