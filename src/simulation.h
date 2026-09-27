

#ifndef FLUID_SIM_V3_SIMULATION_H
#define FLUID_SIM_V3_SIMULATION_H

#include <vector>

struct State {
    float rho = 0.0f;
    float rhoU = 0.0f;
    float rhoV = 0.0f;
    float E = 0.0f;

    float u() const { return rhoU / rho; }
    float v() const { return rhoV / rho; }

    // Ideal gas equation of state: P = (gamma - 1) * (E - 0.5 * rho * |v|^2)
    float pressure(float gamma = 1.4f) const {
        float kineticEnergy = 0.5f * (rhoU * rhoU + rhoV * rhoV) / rho;
        return (gamma - 1.0f) * (E - kineticEnergy);
    }
};

class CFDSimulation {
public:
    CFDSimulation(unsigned int width, unsigned int height);

    void step(float dt);

    unsigned int getWidth() const  { return m_width; }
    unsigned int getHeight() const { return m_height; }

    const std::vector<State>& getStates() const { return m_states; }

private:
    unsigned int m_width;
    unsigned int m_height;

    std::vector<State> m_states;
};

#endif //FLUID_SIM_V3_SIMULATION_H
