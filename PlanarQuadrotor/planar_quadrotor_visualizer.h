#ifndef PLANAR_QUADROTOR_VISUALIZER_H
#define PLANAR_QUADROTOR_VISUALIZER_H

#include "planar_quadrotor.h"
#include "SDL.h"
#include <memory>

class PlanarQuadrotorVisualizer {
public:
    PlanarQuadrotorVisualizer(PlanarQuadrotor* quadrotor_ptr);
    void render(std::shared_ptr<SDL_Renderer>& gRenderer);

private:
    PlanarQuadrotor* quadrotor_ptr;
    void rotatePoint(float& x, float& y, float cx, float cy, float theta); 
};
void write_history(std::vector<float>& x_history, std::vector<float>& y_history, std::vector<float>& theta_history, PlanarQuadrotor& quadrotor);
#endif
