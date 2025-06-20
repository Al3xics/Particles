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


    // Création de lignes aléatoires
    struct Line {
        glm::vec2 p1, p2;
    };

    std::vector<Line> lines;

    int lineCount = 3;
    int minimumLineLenght = 1.f;
    int maximumLineLenght = 1.5f;

    for (int i = 0; i < lineCount; ++i) {
        glm::vec2 center = glm::vec2(
            utils::rand(-gl::window_aspect_ratio() + 0.1f, gl::window_aspect_ratio() - 0.1f),
            utils::rand(-0.9f, 0.9f)
        );

        float angle = utils::rand(0.f, glm::two_pi<float>());
        float length = utils::rand(minimumLineLenght, maximumLineLenght);
        glm::vec2 dir = glm::vec2(std::cos(angle), std::sin(angle)) * (length * 0.5f);

        lines.push_back({ center - dir, center + dir });
    }

    // Ajouter des lignes au bord de l'écran pour éviter que les particules ne sortent
    glm::vec2 topLeft(-gl::window_aspect_ratio(), 1.f);
    glm::vec2 topRight(gl::window_aspect_ratio(), 1.f);
    glm::vec2 bottomLeft(-gl::window_aspect_ratio(), -1.f);
    glm::vec2 bottomRight(gl::window_aspect_ratio(), -1.f);

    lines.push_back({topLeft, topRight});
    lines.push_back({topRight, bottomRight});
    lines.push_back({bottomRight, bottomLeft});
    lines.push_back({bottomLeft, topLeft});

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float dt = gl::delta_time_in_seconds();

        // Afficher les particules
        for (auto it = particles.begin(); it != particles.end(); )
        {
            it->update(dt);

            glm::vec2 intersection;
            bool collided = false;
            glm::vec2 normal;

            glm::vec2 nextPos = it->position + it->velocity * dt;

            for (const auto& line : lines) {
                if (intersect_segments(it->position, nextPos, line.p1, line.p2, intersection)) {
                    collided = true;
                    
                    // Calcul du vecteur directeur de la ligne
                    glm::vec2 edge = line.p2 - line.p1;
                    normal = glm::normalize(glm::vec2(-edge.y, edge.x)); // normale perpendiculaire

                    // Si la normale ne pointe pas vers la particule, on l'inverse
                    if (glm::dot(normal, it->velocity) > 0.f)
                        normal = -normal;

                    break; // On considère la première collision trouvée
                }
            }

            if (collided) {
                // Vélocité réfléchie
                glm::vec2 reflectedVelocity = glm::reflect(it->velocity, normal);

                // Distance parcourue après la collision
                float distAfterIntersection = glm::length(nextPos - intersection);

                // Repositionnement : on recule sur la nouvelle direction
                it->position = intersection + reflectedVelocity * (distAfterIntersection / glm::length(reflectedVelocity));
                it->velocity = reflectedVelocity;
            }

            if (it->isDead()) {
                it = particles.erase(it);
            } else {
                utils::draw_disk(it->position, it->radius(), it->color());
                ++it;
            }
        }

        // Afficher les lignes
        for (const auto& line : lines) {
            utils::draw_line(line.p1, line.p2, 0.005f, glm::vec4(1, 0, 0, 1));
        }
    }
}