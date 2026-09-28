#include "simulation.h"

Simulation::Simulation(Mesh mesh) {
    m_mesh = std::move(mesh);
}
namespace {
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

        const float lamdaMax = std::max(
            std::abs(UnL) + sLeft.speedOfSound(gamma),
            std::abs(UnR) + sRight.speedOfSound(gamma)
        );

        State dampingTerm;
        dampingTerm.rho = 0.5f * lamdaMax * (sRight.rho - sLeft.rho);
        dampingTerm.rhoU.x = 0.5f * lamdaMax * (sRight.rhoU.x - sLeft.rhoU.x);
        dampingTerm.rhoU.y = 0.5f * lamdaMax * (sRight.rhoU.y - sLeft.rhoU.y);
        dampingTerm.E = 0.5f * lamdaMax * (sRight.E - sLeft.E);

        State fluxEdge;
        const State fluxR = getFlux(sRight, normal, gamma);
        const State fluxL = getFlux(sLeft,  normal, gamma);
        fluxEdge.rho = 0.5f * (fluxR.rho + fluxL.rho) - dampingTerm.rho;
        fluxEdge.rhoU.x = 0.5f * (fluxR.rhoU.x + fluxL.rhoU.x) - dampingTerm.rhoU.x;
        fluxEdge.rhoU.y = 0.5f * (fluxR.rhoU.y + fluxL.rhoU.y) - dampingTerm.rhoU.y;
        fluxEdge.E = 0.5f * (fluxR.E + fluxL.E) - dampingTerm.E;

        return fluxEdge;
    }
}

void Simulation::step(const float dt) {
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

        const State flux = riemannSolver(sLeft, sRight, edge.normal, gamma);

        m_mesh.cells[edge.cellLeft].residual.rho   += flux.rho   * edge.length;
        m_mesh.cells[edge.cellLeft].residual.rhoU.x += flux.rhoU.x * edge.length;
        m_mesh.cells[edge.cellLeft].residual.rhoU.y += flux.rhoU.y * edge.length;
        m_mesh.cells[edge.cellLeft].residual.E     += flux.E     * edge.length;

        if (edge.type == BoundaryType::INTERIOR) {
            m_mesh.cells[edge.cellRight].residual.rho   -= flux.rho   * edge.length;
            m_mesh.cells[edge.cellRight].residual.rhoU.x -= flux.rhoU.x * edge.length;
            m_mesh.cells[edge.cellRight].residual.rhoU.y -= flux.rhoU.y * edge.length;
            m_mesh.cells[edge.cellRight].residual.E     -= flux.E     * edge.length;
        }
    }

    for (auto& cell : m_mesh.cells) {
        const float delta = dt / cell.area;

        cell.state.rho = cell.state.rho - (cell.residual.rho * delta);
        cell.state.rhoU.x = cell.state.rhoU.x - (cell.residual.rhoU.x * delta);
        cell.state.rhoU.y = cell.state.rhoU.y - (cell.residual.rhoU.y * delta);
        cell.state.E = cell.state.E - (cell.residual.E * delta);
    }

}
