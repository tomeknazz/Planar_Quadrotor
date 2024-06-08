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

#endif
