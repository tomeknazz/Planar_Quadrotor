#include "simulate.h"
#include "sound.h"
#include "matplot/util/common.h"
#include "matplot/matplot.h"

Eigen::Vector2f transformCoordinates(const int screenX, const int screenY, const int screenWidth, const int screenHeight) {
	const float worldX = 12.0f * (screenX - screenWidth / 2.0f) / screenWidth;
    float worldY = -12.0f * (screenY - screenHeight / 2.0f) / screenHeight;
    return Eigen::Vector2f(worldX, worldY);
}

Eigen::MatrixXf LQR(PlanarQuadrotor& quadrotor, float dt) {
    Eigen::MatrixXf Eye = Eigen::MatrixXf::Identity(6, 6);
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(6, 6);
    Eigen::MatrixXf A_discrete = Eigen::MatrixXf::Zero(6, 6);
    Eigen::MatrixXf B(6, 2);
    Eigen::MatrixXf B_discrete(6, 2);
    Eigen::MatrixXf Q = Eigen::MatrixXf::Identity(6, 6);
    Eigen::MatrixXf R = Eigen::MatrixXf::Identity(2, 2);
    Eigen::MatrixXf K = Eigen::MatrixXf::Zero(6, 6);
    Eigen::Vector2f input = quadrotor.GravityCompInput();

    Q.diagonal() << 1, 10, 0, 0, 1,1 / M_PI;
    R.row(0) << 30, 7;
    R.row(1) << 7, 30;

    std::tie(A, B) = quadrotor.Linearize();
    A_discrete = Eye + dt * A;
    B_discrete = dt * B;

    return LQR(A_discrete, B_discrete, Q, R);
}

void control(PlanarQuadrotor& quadrotor, const Eigen::MatrixXf& K) {
    Eigen::Vector2f input = quadrotor.GravityCompInput();
    quadrotor.SetInput(input - K * quadrotor.GetControlState());
}

int main(int argc, char* args[])
{
    std::shared_ptr<SDL_Window> gWindow = nullptr;
    std::shared_ptr<SDL_Renderer> gRenderer = nullptr;
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 720;

    Sound sound("Drone.wav");
    sound.Tool();
    sound.Go();
    


    Eigen::VectorXf initial_state = Eigen::VectorXf::Zero(6);
    PlanarQuadrotor quadrotor(initial_state);
    PlanarQuadrotorVisualizer quadrotor_visualizer(&quadrotor);
    Eigen::VectorXf goal_state = Eigen::VectorXf::Zero(6);
    goal_state << 0, 0, 0, 0, 0, 0;
    quadrotor.SetGoal(goal_state);
    constexpr float dt = 0.001;
    Eigen::MatrixXf K = LQR(quadrotor, dt);
    Eigen::Vector2f input = Eigen::Vector2f::Zero(2);

    if (init(gWindow, gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT) >= 0)
    {
	    std::vector<float> theta_history;
	    std::vector<float> y_history;
	    std::vector<float> x_history;
	    SDL_Event e;
        bool quit = false;
        int x, y;
        Eigen::VectorXf state = Eigen::VectorXf::Zero(6);

        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&x, &y);
                    Eigen::Vector2f goal = transformCoordinates(x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
                    goal_state << goal.x(), goal.y(), 0, 0, 0, 0;
                    quadrotor.SetGoal(goal_state);
                    std::cout << "Goal state set to: (" << goal_state.x() << ", " << goal_state.y() << ")" << '\n';
                }
                else if (e.type == SDL_MOUSEMOTION)
                {
                    SDL_GetMouseState(&x, &y);
                    std::cout << "Mouse position: (" << x << ", " << y << ")" << '\n';
                }
                else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)
                {
                    using namespace matplot;
                    hold(on);
                    plot(x_history);
                    plot(y_history);
                	plot(theta_history);
                    title("Quadrotor position");
                    xlabel("Time");
                    ylabel("Value");
                    legend({"x", "y", "theta"});
                    hold(off);
                    show();

                }
            }
            write_history(x_history, y_history, theta_history, quadrotor);
            SDL_Delay(static_cast<int>(dt * 1000));
            SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer.get());
            quadrotor_visualizer.render(gRenderer);
            SDL_RenderPresent(gRenderer.get());
            control(quadrotor, K);
            quadrotor.Update(dt);
        }
    }
    SDL_Quit();
    sound.End();
    return 0;
}

int init(std::shared_ptr<SDL_Window>& gWindow, std::shared_ptr<SDL_Renderer>& gRenderer, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
    if (SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        gWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow("Planar Quadrotor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
        gRenderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(gWindow.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
        SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        std::cout << "SDL_ERROR: " << SDL_GetError() << '\n';
        return -1;
    }
    return 0;
}
