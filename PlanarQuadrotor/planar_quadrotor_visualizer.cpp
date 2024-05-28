#include "planar_quadrotor_visualizer.h"
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include <Eigen/Dense>
#include <cmath>
#include <chrono>

PlanarQuadrotorVisualizer::PlanarQuadrotorVisualizer(PlanarQuadrotor* quadrotor_ptr)
    : quadrotor_ptr(quadrotor_ptr) {}

void PlanarQuadrotorVisualizer::render(std::shared_ptr<SDL_Renderer>& gRenderer) {
    Eigen::VectorXf state = quadrotor_ptr->GetState();
    float q_x, q_y, q_theta;

    //wyśrdokowywanie
    q_x = state[0];
    q_y = state[1];
    q_theta = state[2];
    float scale = 10.0f;
    int x = static_cast<int>(640 + q_x * scale); 
    int y = static_cast<int>(360 - q_y * scale); 

    //ciało drona
    SDL_SetRenderDrawColor(gRenderer.get(), 0x80, 0x80, 0x80, 0xFF); 
    int body_width = 120;
    int body_height = 20;
    SDL_Rect body_rect = { x - body_width / 2, y - body_height / 2, body_width, body_height };
    SDL_RenderFillRect(gRenderer.get(), &body_rect);

    //odnogi xd?
    SDL_SetRenderDrawColor(gRenderer.get(), 0x50, 0x50, 0x50, 0xFF); 
    int leg_width = 6;
    int leg_height = 30;
    int leg_offset = body_height / 2 + leg_height / 2;
    SDL_Rect leg1_rect = { x - body_width / 2 - leg_width / 2, y - leg_offset, leg_width, leg_height };
    SDL_Rect leg2_rect = { x + body_width / 2 - leg_width / 2, y - leg_offset, leg_width, leg_height };
    SDL_RenderFillRect(gRenderer.get(), &leg1_rect);
    SDL_RenderFillRect(gRenderer.get(), &leg2_rect);

    //śmigła
    int propeller_width = 30; 
    int propeller_height = 7; 
    int propeller_offset = 75; 
    using namespace std;
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    double angle = millis / 50.0;

    int propeller1_x = static_cast<int>(x + propeller_offset * cos(angle));
    int propeller1_y = y - 20;
    int propeller2_x = static_cast<int>(x - propeller_offset * cos(angle));
    int propeller2_y = y - 20;

    filledEllipseColor(gRenderer.get(), propeller1_x, propeller1_y, propeller_width / 2, propeller_height / 2, 0xFFFF0000);
    filledEllipseColor(gRenderer.get(), propeller2_x, propeller2_y, propeller_width / 2, propeller_height / 2, 0xFFFF0000);
}
