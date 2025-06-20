#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include "Struct/Particles.hpp"
#include <vector>
#include <cstdlib> // Pour std::rand et std::srand
#include <ctime>   // Pour std::time


// p1 + (t * r) = q1 + (u * s)
bool intersect_segments(glm::vec2 p1, glm::vec2 p2, glm::vec2 q1, glm::vec2 q2, glm::vec2& intersection)
{
    // ------ Calcul des vecteur directeur ------
    glm::vec2 r = p2 - p1; // Vecteur directeur du segment p (calculée avec les extrémités p1 et p2 du segment p)
    glm::vec2 s = q2 - q1; // Vecteur directeur du segment q (calculée avec les extrémités q1 et q2 du segment q)
    
    // ------ Calcul du déterminant ------
    glm::mat2x2 m{ r, -s }; // Matrice formée par les vecteurs r et -s
    glm::vec2 t = glm::inverse(m) * (q1-p1);

    // ------ Vérification de si le point sur l'intersection est dans les segments ------
    if (t.x >= 0 && t.x <= 1 && t.y >= 0 && t.y <= 1) {
        intersection = p1 + t * r;
        return true;
    }

    return false;
}


int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Créer un tableau de particules
    std::vector<Particle> particles(200);

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float dt = gl::delta_time_in_seconds();

        // // Afficher les particules
        // for (auto it = particles.begin(); it != particles.end(); )
        // {
        //     it->update(dt);
        //     if (it->isDead()) {
        //         it = particles.erase(it);
        //     } else {
        //         utils::draw_disk(it->position, it->radius(), it->color());
        //         ++it;
        //     }
        // }


        // Debug intersection entre 2 lignes
        float margin = 0.3f * gl::window_aspect_ratio();

        // Ligne horizontale centrée
        glm::vec2 center_left = {-gl::window_aspect_ratio() + margin, 0.0f};
        glm::vec2 center_right = { gl::window_aspect_ratio() - margin, 0.0f};
        utils::draw_line(center_left, center_right, 0.005f, glm::vec4(1.f, 1.f, 1.f, 1.f));

        // Ligne du bas vers la souris
        glm::vec2 bottom_center = {0.0f, -1.0f};
        glm::vec2 mouse = gl::mouse_position();
        utils::draw_line(bottom_center, mouse, 0.005f, glm::vec4(1.f, 1.f, 1.f, 1.f));

        // Dessiner le point d'intersection
        glm::vec2 intersection;
        bool found = intersect_segments(center_left, center_right, bottom_center, mouse, intersection);
        if (found) {
            utils::draw_disk(intersection, 0.015f, glm::vec4(1.f, 1.f, 0.f, 1.f)); // Jaune
        }
    }
}