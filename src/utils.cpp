#include "utils.hpp"
#include <random>
#include "opengl-framework/opengl-framework.hpp"
#include <glm/gtc/constants.hpp>

namespace utils {

static auto& generator()
{
    thread_local std::default_random_engine gen{std::random_device{}()};
    return gen;
}

float rand(float min, float max)
{
    return std::uniform_real_distribution<float>{min, max}(generator());
}

static auto make_square_mesh() -> gl::Mesh
{
    return gl::Mesh{gl::Mesh_Descriptor{
        .vertex_buffers = {
            gl::VertexBuffer_Descriptor{
                .layout = {gl::VertexAttribute::Position2D(0), gl::VertexAttribute::UV(1)},
                .data   = {
                    -1.f, -1.f, 0.f, 0.f, //
                    +1.f, -1.f, 1.f, 0.f, //
                    +1.f, +1.f, 1.f, 1.f, //
                    -1.f, +1.f, 0.f, 1.f  //
                }
            }
        },
        .index_buffer = {0, 1, 2, 0, 2, 3},
    }};
}

static auto make_disk_shader() -> gl::Shader
{
    return gl::Shader{
        gl::Shader_Descriptor{
            .vertex   = gl::ShaderSource::Code({R"GLSL(
#version 410

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

uniform vec2 u_position;
uniform float u_radius;
uniform float u_inverse_aspect_ratio;

out vec2 v_uv;

void main()
{
    vec2 position = u_position + u_radius * in_position;

    gl_Position = vec4(position * vec2(u_inverse_aspect_ratio, 1.), 0., 1.);
    v_uv = in_uv;
}
)GLSL"}),
            .fragment = gl::ShaderSource::Code({R"GLSL(
#version 410

out vec4 out_color;

in vec2 v_uv;
uniform vec4 u_color;

void main()
{
    vec2 dir = v_uv - vec2(0.5);
    if (dot(dir, dir) > 0.25)
        discard;
    out_color = u_color;
}
)GLSL"}),
        }
    };
}

void draw_disk(glm::vec2 position, float radius, glm::vec4 const& color)
{
    static auto square_mesh = make_square_mesh();
    static auto disk_shader = make_disk_shader();

    disk_shader.bind();
    disk_shader.set_uniform("u_position", position);
    disk_shader.set_uniform("u_radius", radius);
    disk_shader.set_uniform("u_inverse_aspect_ratio", 1.f / gl::framebuffer_aspect_ratio());
    disk_shader.set_uniform("u_color", color);
    square_mesh.draw();
}

static auto make_line_shader() -> gl::Shader
{
    return gl::Shader{
        gl::Shader_Descriptor{
            .vertex   = gl::ShaderSource::Code({R"GLSL(
#version 410

uniform vec2 u_start;
uniform vec2 u_end;
uniform float u_thickness;
uniform float u_inverse_aspect_ratio;

const vec2 quadOffsets[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

void main() {
    // Line direction and normal
    vec2 dir = normalize(u_end - u_start);
    vec2 normal = vec2(-dir.y, dir.x);

    vec2 middle = (u_start + u_end) * 0.5;
    vec2 pos = middle
             + quadOffsets[gl_VertexID].x * (u_end - u_start) * 0.5
             + quadOffsets[gl_VertexID].y * normal * u_thickness * 0.5;

    gl_Position = vec4(pos * vec2(u_inverse_aspect_ratio, 1.), 0., 1.);
}
)GLSL"}),
            .fragment = gl::ShaderSource::Code({R"GLSL(
#version 410

out vec4 out_color;
uniform vec4 u_color;

void main()
{
    out_color = u_color;
}
)GLSL"}),
        }
    };
}

void draw_line(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 const& color)
{
    static auto line_mesh   = make_square_mesh();
    static auto line_shader = make_line_shader();
    line_shader.bind();
    line_shader.set_uniform("u_start", start);
    line_shader.set_uniform("u_end", end);
    line_shader.set_uniform("u_thickness", thickness);
    line_shader.set_uniform("u_inverse_aspect_ratio", 1.f / gl::framebuffer_aspect_ratio());
    line_shader.set_uniform("u_color", color);
    line_mesh.draw();
}

std::vector<glm::vec2> poisson_disc_sampling(glm::vec2 center, float radius, float minDist, int k) {
    float cellSize = minDist / std::sqrt(2.f);
    int gridSize = static_cast<int>(std::ceil((2 * radius) / cellSize));

    // Grille 2D (flattened), -1 = pas de point
    std::vector<int> grid(gridSize * gridSize, -1);

    std::vector<glm::vec2> samples;
    std::vector<glm::vec2> activeList;

    // Place initial point au centre
    samples.push_back(center);
    activeList.push_back(center);

    // Fonction pour convertir une position en index de grille
    auto getGridIndex = [&](glm::vec2 point) -> glm::ivec2 {
        glm::vec2 local = point - (center - glm::vec2(radius));
        return glm::ivec2(local / cellSize);
    };

    // Place le point initial dans la grille
    glm::ivec2 index = getGridIndex(center);
    grid[index.x + index.y * gridSize] = 0;

    while (!activeList.empty()) {
        int randIndex = std::rand() % activeList.size();
        glm::vec2 current = activeList[randIndex];
        bool found = false;

        for (int i = 0; i < k; ++i) {
            float r = utils::rand(minDist, 2.f * minDist);
            float angle = utils::rand(0.f, 2.f * glm::pi<float>());

            glm::vec2 offset = glm::vec2(std::cos(angle), std::sin(angle)) * r;
            glm::vec2 candidate = current + offset;

            if (glm::distance(candidate, center) > radius) continue;

            glm::ivec2 gridIdx = getGridIndex(candidate);
            if (gridIdx.x < 0 || gridIdx.y < 0 || gridIdx.x >= gridSize || gridIdx.y >= gridSize)
                continue;

            bool tooClose = false;

            // Vérifie les voisins
            for (int dx = -2; dx <= 2 && !tooClose; ++dx) {
                for (int dy = -2; dy <= 2 && !tooClose; ++dy) {
                    int nx = gridIdx.x + dx;
                    int ny = gridIdx.y + dy;

                    if (nx < 0 || ny < 0 || nx >= gridSize || ny >= gridSize) continue;

                    int neighborIndex = grid[nx + ny * gridSize];
                    if (neighborIndex >= 0) {
                        if (glm::distance(candidate, samples[neighborIndex]) < minDist) {
                            tooClose = true;
                        }
                    }
                }
            }

            if (!tooClose) {
                samples.push_back(candidate);
                activeList.push_back(candidate);
                grid[gridIdx.x + gridIdx.y * gridSize] = samples.size() - 1;
                found = true;
                break;
            }
        }

        if (!found) {
            // Aucun point trouvé autour de current : on le retire
            activeList.erase(activeList.begin() + randIndex);
        }
    }

    return samples;
}

} // namespace utils