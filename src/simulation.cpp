#include "simulation.h"

Simulation::Simulation(Mesh mesh) {
    m_mesh = std::move(mesh);
}

State wallState(const State sLeft, const Vec2 normal) {
    State sRight;

    sRight.rho = sLeft.rho;
    sRight.E = sLeft.E;

    float rhoUn = (sLeft.rhoU.x * normal.x) + (sLeft.rhoU.y * normal.y);

    sRight.rhoU.x = sLeft.rhoU.x - (2 * rhoUn * normal.x);
    sRight.rhoU.y = sLeft.rhoU.y - (2 * rhoUn * normal.y);

    return sRight;
}

State getFlux(const State state, const Vec2 normal, const float gamma) {
    State flux;

    const Vec2 u = state.u();
    const float pressure = state.pressure(gamma);

    const float Un = (u.x * normal.x) + (u.y * normal.y);

    flux.rho = state.rho * Un;

    flux.rhoU.x = (state.rhoU.x * Un) + (pressure * normal.x);
    flux.rhoU.y = (state.rhoU.y * Un) + (pressure * normal.y);

    flux.E = (state.E + pressure) * Un;

    return flux;
}

State riemannSolver(const State sLeft, const State sRight, const Vec2 normal, const float gamma) {
    const Vec2 uL = sLeft.u();
    const Vec2 uR = sRight.u();

    const float UnL = (uL.x * normal.x) + (uL.y * normal.y);
    const float UnR = (uR.x * normal.x) + (uR.y * normal.y);

    const float lamdaMax = std::max(UnL + sLeft.speedOfSound(), UnR + sRight.speedOfSound());

}

void Simulation::step(float dt) {
    //clear residuals
    for (auto& cell : m_mesh.cells) {
        cell.residual = State{};
    }

    for (auto& edge : m_mesh.edges) {
        State sLeft = m_mesh.cells[edge.cellLeft].state;
        State sRight;

        switch(edge.type){
            case BoundaryType::WALL:
                sRight = wallState(sLeft, edge.normal);
                break;
            case BoundaryType::INFLOW:
                sRight = uFreestream;
                break;
            case BoundaryType::OUTFLOW:
                sRight = sLeft;
                break;
            default:
                sRight = m_mesh.cells[edge.cellRight].state;
                break;
        };

        State flux = riemannSolver(sLeft, sRight, edge.normal, gamma);
    }
}
