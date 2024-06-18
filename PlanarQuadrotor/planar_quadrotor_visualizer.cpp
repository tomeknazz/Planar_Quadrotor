#include "planar_quadrotor_visualizer.h"
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include <Eigen/Dense>
#include <cmath>
#include <chrono>

PlanarQuadrotorVisualizer::PlanarQuadrotorVisualizer(PlanarQuadrotor* quadrotor_ptr)
	: quadrotor_ptr(quadrotor_ptr) {}

void PlanarQuadrotorVisualizer::rotatePoint(float& x, float& y, float cx, float cy, float theta) {
	// Przesunięcie do środka układu współrzędnych
	const float temp_x = x - cx;
	const float temp_y = y - cy;

	// Obrót punktu
	x = temp_x * cos(theta) - temp_y * sin(theta) + cx;
	y = temp_x * sin(theta) + temp_y * cos(theta) + cy;
}

void write_history(std::vector<float>& x_history, std::vector<float>& y_history, std::vector<float>& theta_history, PlanarQuadrotor& quadrotor) {
	Eigen::VectorXf state = quadrotor.GetState();
	x_history.push_back(state[0]);
	y_history.push_back(state[1]);
	theta_history.push_back(state[2]/2);
}

void PlanarQuadrotorVisualizer::render(std::shared_ptr<SDL_Renderer>& gRenderer) {
	Eigen::VectorXf state = quadrotor_ptr->GetState();


	// Wyśrodkowanie
	const float q_x = state[0];
	const float q_y = state[1];
	const float q_theta = state[2] / 2;
	constexpr float scale = 10.0f;
	const int x = static_cast<int>(640 + q_x * scale);
	const int y = static_cast<int>(360 - q_y * scale);

	SDL_Texture* body_texture = SDL_CreateTexture(gRenderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
	constexpr Uint32 pixel = 0xFFFFF9FF; // White-ish color
	SDL_UpdateTexture(body_texture, nullptr, &pixel, sizeof(pixel));
	SDL_SetTextureColorMod(body_texture, 100, 100, 150);


	constexpr int body_width = 120;
	constexpr int body_height = 20;
	const float body_x = x - body_width / 2;
	const float body_y = y - body_height / 2;


	const SDL_Rect body_rect = { static_cast<int>(body_x), static_cast<int>(body_y), body_width, body_height };
	SDL_RenderCopyEx(gRenderer.get(), body_texture, nullptr, &body_rect, -q_theta * (180 / M_PI), nullptr, SDL_FLIP_NONE);


	constexpr int leg_width = 6;
	constexpr int leg_height = 30;
	constexpr int leg_offset = body_height / 2 + leg_height / 2;
	float leg1_x = x - body_width / 2 - leg_width / 2;
	float leg1_y = y - leg_offset;
	float leg2_x = x + body_width / 2 - leg_width / 2;
	float leg2_y = y - leg_offset;

	rotatePoint(leg1_x, leg1_y, x, y, -q_theta);
	rotatePoint(leg2_x, leg2_y, x, y, -q_theta);

	const SDL_Rect leg1_rect = { static_cast<int>(leg1_x), static_cast<int>(leg1_y), leg_width, leg_height };
	const SDL_Rect leg2_rect = { static_cast<int>(leg2_x), static_cast<int>(leg2_y), leg_width, leg_height };

	SDL_RenderCopyEx(gRenderer.get(), body_texture, nullptr, &leg1_rect, -q_theta * (180 / M_PI), nullptr, SDL_FLIP_NONE);
	SDL_RenderCopyEx(gRenderer.get(), body_texture, nullptr, &leg2_rect, -q_theta * (180 / M_PI), nullptr, SDL_FLIP_NONE);


	constexpr int propeller_width = 25;
	constexpr int propeller_height = 7;
	constexpr int propeller_offset = 90;
	const auto now = std::chrono::system_clock::now();
	const auto duration = now.time_since_epoch();
	const double millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	const double angle = millis / 25.0;

	float propeller1_x = (x + propeller_offset * cos(angle) - 30);
	float propeller1_y = y - 20;
	float propeller2_x = (x - propeller_offset * cos(angle) + 4);
	float propeller2_y = y - 20;

	rotatePoint(propeller1_x, propeller1_y, x, y, -q_theta);
	rotatePoint(propeller2_x, propeller2_y, x, y, -q_theta);

	const SDL_Rect wing1_rect = { static_cast<int>(propeller1_x), static_cast<int>(propeller1_y), propeller_width, propeller_height };
	const SDL_Rect wing2_rect = { static_cast<int>(propeller2_x), static_cast<int>(propeller2_y), propeller_width, propeller_height };

	SDL_RenderCopyEx(gRenderer.get(), body_texture, nullptr, &wing1_rect, -q_theta * (180 / M_PI), nullptr, SDL_FLIP_NONE);
	SDL_RenderCopyEx(gRenderer.get(), body_texture, nullptr, &wing2_rect, -q_theta * (180 / M_PI), nullptr, SDL_FLIP_NONE);
}
