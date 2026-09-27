

#ifndef FLUID_SIM_V3_SIMULATION_H
#define FLUID_SIM_V3_SIMULATION_H

#include <vector>
#include <cmath>

enum class BoundaryType {
    INTERIOR,
    WALL,
    INFLOW,
    OUTFLOW
};

struct Vec2 {
    float x;
    float y;
};

struct State {
    float rho = 0.0f;
    Vec2 rhoU{0.0f, 0.0f};
    float E = 0.0f;

    Vec2 u() const { return Vec2{rhoU.x / rho, rhoU.y / rho}; }

    // Ideal gas equation of state: P = (gamma - 1) * (E - 0.5 * rho * |v|^2)
    float pressure(const float gamma = 1.4f) const {
        const float kineticEnergy = 0.5f * (rhoU.x * rhoU.x + rhoU.y * rhoU.y) / rho;
        return (gamma - 1.0f) * (E - kineticEnergy);
    }

    float speedOfSound(const float gamma = 1.4f) const {
        const float p = pressure(gamma);
        return std::sqrt(gamma * std::max(p, 1e-6f) / rho);
    }
};

struct Edge {
    float length = 0.0f;
    Vec2 normal = Vec2{0.0f, 0.0f};

    signed int cellLeft = 0;
    signed int cellRight = -1;

    BoundaryType type = BoundaryType::INTERIOR;

    int nodeA = 0;
    int nodeB = 0;
};

struct Cell {
    float area = 0.0f;
    State state;
    State residual;

    Vec2 centroid;
};

struct Mesh {
    std::vector<Cell> cells;
    std::vector<Edge> edges;

    std::vector<Vec2> nodes;
};

class Simulation {
public:
    explicit Simulation(Mesh mesh);

    void step(float dt);

    const Mesh& getMesh() const { return m_mesh; }

    State uFreestream;

    float gamma = 1.4f;

private:
    Mesh m_mesh;
};

#endif //FLUID_SIM_V3_SIMULATION_H
