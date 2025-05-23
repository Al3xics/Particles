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

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float dt = gl::delta_time_in_seconds();

        // Afficher les particules
        for (auto it = particles.begin(); it != particles.end(); )
        {
            it->update(dt);
            if (it->isDead()) {
                it = particles.erase(it);
            } else {
                utils::draw_disk(it->position, it->radius(), it->color());
                ++it;
            }
        }
    }
}