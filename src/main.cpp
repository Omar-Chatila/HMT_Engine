/*
    Test class for later DTF/Frechet implementations
*/

#include <GL/glu.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "../include/dtw.h"
#include "../include/window_helper.h"

using std::pair, std::vector;

// WINDOW
const int WIDTH = 1000;
const int HEIGHT = 900;
const int rect_size = 4;
constexpr int rect_offset = rect_size / 2;

const int n = 6;
const int m = 7;
vector<double *> frames1;
vector<double *> frames2;
double vals1[n] = {0, 2, 0, 1, 0, 0};
double vals2[m] = {0, 2, 0, 1, 0, 0};
double vals3[m] = {0, 0, 0.5, 2, 0, 1, 0};

double draw_curves(SDL_Renderer *rend, double *vals1, double *vals2, int x, int y, double (*func)(double, double));
void display_text(SDL_Renderer *rend, const char *message, int x, int y);

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("DTW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    SDL_Event event;
    SDL_SetRenderDrawColor(rend, 222, 222, 222, 222);
    // vector<vector<double>> curves = get_curves_from_csv();
    Dtw::set_frames(vals1, vals2, m, n, frames1, frames2);

    int frame = 0;
    int max_frame = frames1.size();
    bool quit = false;
    bool calculated = false;
    std::string title = "(" + std::to_string(frame + 1) + "/" + std::to_string(max_frame) + ")";
    SDL_SetWindowTitle(window, title.c_str());
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            auto code = event.key.keysym.scancode;
            if (code == SDL_SCANCODE_ESCAPE || event.type == SDL_QUIT) {
                quit = true;
            } else if (code == SDL_SCANCODE_RIGHT && event.type == SDL_KEYDOWN) {
                calculated = false;
                frame++;
                std::string title = "(" + std::to_string(frame + 1) + "/" + std::to_string(max_frame) + ")";
                SDL_SetWindowTitle(window, title.c_str());
            }
        }

        if (!calculated){
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderClear(rend);
            // Abs
            double cost = draw_curves(rend, frames1[frame], frames2[frame], 0, 0, diff);
            std::string message = std::string("Diff") + ": " + std::to_string(cost);
            const char* cstr = message.c_str();
            display_text(rend, cstr, 0, 0);

            // Abs
            double cost2 = draw_curves(rend, frames1[frame], frames2[frame], 400, 0, squared_euclid);
            std::string message2 = std::string("Squared euclid: ") + ": " + std::to_string(cost2);
            const char* cstr2 = message2.c_str();
            display_text(rend, cstr2, 400, 0);
            calculated = true;

            SDL_RenderPresent(rend);
        }
        if (frame == max_frame - 1) quit = true;
    }

    for (double* arr : frames1) {
        free(arr);
    }
    for (double* arr : frames2) {
        free(arr);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(rend);
    SDL_Quit();
    return 0;
}

void draw_hmt_example(SDL_Renderer *rend, SDL_Event &event, vector<vector<double>> &curves, int max_frame, int frame) {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
    SDL_RenderClear(rend);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            break;
        }
    }
    for (int i = 0; i < curves.size(); i += 3) {
        if (frame < max_frame) {
            double x = curves[i][frame] * 10;
            double y = curves[i + 1][frame] * 10;
            double z = curves[i + 2][frame] * 10;

            double scaleFactor = 300.0 / (300.0 + z);
            int screenX = WIDTH / 2 + static_cast<int>(x * scaleFactor);
            int screenY = HEIGHT / 2 - static_cast<int>(y * scaleFactor);

            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            SDL_RenderDrawPoint(rend, screenX, screenY);
            SDL_RenderDrawPoint(rend, screenX, screenY);
        }
    }
    SDL_RenderPresent(rend);
    SDL_Delay(1000 / 10);
    frame++;
    if (frame >= max_frame)
        frame = 0;
}

double draw_curves(SDL_Renderer *rend, double *vals1, double *vals2, int x, int y, double (*func)(double, double)) {
    vector<SDL_Rect> curve1;
    vector<SDL_Rect> curve2;
    auto result = Dtw::run_example(vals1, vals2, n, m, func);
    const int xOffset = x + 50;
    const int yOffset = y + 250;
    for (int i = 0; i < n; i++) {
        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
        SDL_Rect rect1(xOffset + i * 50, yOffset - 50 * vals1[i], 10, 10);
        curve1.push_back(rect1);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect1.x + rect_offset, rect1.y + rect_offset, curve1[i - 1].x + rect_offset, curve1[i - 1].y + rect_offset);
        SDL_RenderDrawRect(rend, &rect1);
    }

    for (int i = 0; i < m; i++) {
        SDL_SetRenderDrawColor(rend, 0, 0, 200, 255);
        SDL_Rect rect2(xOffset + i * 50, yOffset + 150 - 50 * vals2[i], 10, 10);
        curve2.push_back(rect2);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect2.x + rect_offset, rect2.y + rect_offset, curve2[i - 1].x + rect_offset, curve2[i - 1].y + rect_offset);
        SDL_RenderDrawRect(rend, &rect2);
    }

    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    // Draw alignment
    for (auto index : result.second) {
        index = index;
        std::cout << "I:" << index << ": ";
        int first = index / (m + 1) - 1;
        int second = index % (m + 1) - 1;
        std::cout << first << "-" << second << '\n';
        SDL_RenderDrawLine(rend, curve1[first].x + rect_offset, curve1[first].y + rect_offset, curve2[second].x + rect_offset, curve2[second].y + rect_offset);
    }

    return result.first;
}