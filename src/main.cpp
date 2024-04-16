#include <iostream>
#include "../include/dtw.h"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <SDL_opengl.h>
#include <GL/glu.h>

using std::pair, std::vector;

// WINDOW
const int WIDTH = 1000;
const int HEIGHT = 900;

const int n = 6;
const int m = 7;
double vals1[n] = {0, 2, 0, 1, 0, 0};
double vals2[m] = {0, 0, 0.5, 2, 0, 1, 0};

double (*func)(double a, double b) = squared_euclid;
vector<vector<double>> get_curves_from_csv();
void draw_example(SDL_Renderer *rend);

pair<double, vector<int>> run_example();

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("DTW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    pair<double, vector<int>> result = run_example();
    SDL_Event event;
    SDL_SetRenderDrawColor(rend, 222, 222, 222, 222);
    vector<vector<double>> curves = get_curves_from_csv();

    int frame = 0;
    int max_frame = curves.size();
    bool quit = false;
    draw_example(rend);
    while (!quit)
    {
        SDL_RenderPresent(rend);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(rend);
    SDL_Quit();
    return 0;
}

void draw_hmt_example(SDL_Renderer *rend, SDL_Event event, vector<vector<double>> curves, int max_frame, int frame) {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                break;
            }
        }
        for (int i = 0; i < curves.size(); i += 3)
        {
            if (frame < max_frame)
            {
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

pair<double, vector<int>> run_example() {
    auto *matrix = Dtw::dtw(vals1, vals2, n, m, func);
    for (int i = 0; i < (n + 1) * (m + 1); i++) {
        auto num = matrix[i];
        // if (i % (m + 1) != 0 && (i / (m + 1)) != 0)
        std::cout << num << ", ";
        if (i % (m + 1) == m) {
            std::cout << '\n';
        }
    }
    auto pair = Dtw::get_cost_and_alignment(matrix, m, n);
    std::cout << "Alignment: " << std::endl;
    for (auto num : pair.second) {
        std::cout << num << " ";
    }
    std::cout << "\nCost: " << pair.first << std::endl;
    delete[] matrix;
    return pair;
}

vector<vector<double>> get_curves_from_csv() {
    vector<vector<double>> curves;
    std::ifstream movement_data("../bvh_parser/walk1.csv");
    std::string myline;
    int index = 0;
    if (movement_data.is_open())
    {
        while (movement_data)
        {
            std::getline(movement_data, myline);
            std::stringstream ss(myline);
            std::string token;
            std::vector<std::string> line;

            while (std::getline(ss, token, ','))
            {
                line.push_back(token);
                // No rotation data for now
            }

            if (!line[0].contains("rot") && !line[0].contains("Frames"))
            {
                index++;
                vector<double> curve_points;
                for (int i = 1; i < line.size(); i++)
                {
                    double num = std::stod(line[i]);
                    curve_points.push_back(num);
                }
                curves.push_back(curve_points);
            }
        }
    }
    else
    {
        std::cerr << "Error opening file\n";
    }
    return curves;
}

void draw_example(SDL_Renderer *rend)
{
    vector<SDL_Rect> curve1;
    vector<SDL_Rect> curve2;
    auto result = run_example();
    int rect_size = 5;
    const int xOffset = 200;
    const int yOffset = HEIGHT / 2;
    for (int i = 0; i < n; i++)
    {
        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
        SDL_Rect rect1(xOffset + i * 50, yOffset - 50 * vals1[i], 10, 10);
        curve1.push_back(rect1);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect1.x + rect_size / 2, rect1.y + rect_size / 2, curve1[i - 1].x + rect_size / 2, curve1[i - 1].y + rect_size / 2);
        SDL_RenderDrawRect(rend, &rect1);
    }

    for (int i = 0; i < m; i++)
    {

        SDL_SetRenderDrawColor(rend, 0, 0, 200, 255);
        SDL_Rect rect2(xOffset + i * 50, yOffset + 150 - 50 * vals2[i], 10, 10);
        curve2.push_back(rect2);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect2.x + rect_size / 2, rect2.y + rect_size / 2, curve2[i - 1].x + rect_size / 2, curve2[i - 1].y + rect_size / 2);
        SDL_RenderDrawRect(rend, &rect2);
    }

    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    // Draw alignment
    for (auto index : result.second)
    {
        index = index;
        std::cout << "I:" << index << ": ";
        int first = index / (m + 1) - 1;
        int second = index % (m + 1) - 1;
        std::cout << first << "-" << second << '\n';
        SDL_RenderDrawLine(rend, curve1[first].x + rect_size / 2, curve1[first].y + rect_size / 2, curve2[second].x + rect_size / 2, curve2[second].y + rect_size / 2);
    }
}

void opengl_example()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("3D Cube",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

        // Draw a cube (die)
        glBegin(GL_QUADS);

        // Front face
        glColor3f(1.0, 0.0, 0.0); // Red
        glVertex3f(-1.0, -1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);

        // Back face
        glColor3f(0.0, 1.0, 0.0); // Green
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(1.0, -1.0, -1.0);

        // Left face
        glColor3f(0.0, 0.0, 1.0); // Blue
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f(-1.0, -1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, -1.0);

        // Right face
        glColor3f(1.0, 1.0, 0.0); // Yellow
        glVertex3f(1.0, -1.0, -1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);

        // Top face
        glColor3f(1.0, 0.0, 1.0); // Magenta
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, -1.0);

        // Bottom face
        glColor3f(0.0, 1.0, 1.0); // Cyan
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f(1.0, -1.0, -1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glVertex3f(-1.0, -1.0, 1.0);

        glEnd();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}