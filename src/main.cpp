#include <GL/glu.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../include/dtw.h"

using std::pair, std::vector;

// WINDOW
const int WIDTH = 1000;
const int HEIGHT = 900;

const int n = 6;
const int m = 7;
vector<double *> frames1;
vector<double *> frames2;
double vals1[n] = {0, 2, 0, 1, 0, 0};
double vals2[m] = {0, 2, 0, 1, 0, 0};
double vals3[m] = {0, 0, 0.5, 2, 0, 1, 0};

double (*func)(double a, double b) = squared_euclid;
vector<vector<double>> get_curves_from_csv();
void draw_curves(SDL_Renderer *rend, double *vals1, double *vals2);
void set_frames();

pair<double, vector<int>> run_example(double *vals1, double *vals2);

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
    set_frames();

    for (auto &curve : frames1) {
        for (int i = 0; i < n; i++) {
            std::cout << curve[i] << ", ";
        }
        std::cout << std::endl;
    }

    for (auto &curve : frames2) {
        for (int i = 0; i < n; i++) {
            std::cout << curve[i] << ", ";
        }
        std::cout << std::endl;
    }

    int frame = 0;
    int max_frame = frames1.size();
    bool quit = false;
    while (!quit) {
        SDL_RenderPresent(rend);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        draw_curves(rend, frames1[frame], frames2[frame]);
        free(frames1[frame]);
        free(frames2[frame]);
        frame++;
        if (frame == max_frame)
            frame = 0;
        SDL_RenderPresent(rend);
        SDL_Delay(1000);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
        SDL_RenderClear(rend);
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

pair<double, vector<int>> run_example(double *vals1, double *vals2) {
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
    if (movement_data.is_open()) {
        while (movement_data) {
            std::getline(movement_data, myline);
            std::stringstream ss(myline);
            std::string token;
            std::vector<std::string> line;

            while (std::getline(ss, token, ',')) {
                line.push_back(token);
                // No rotation data for now
            }

            if (!line[0].contains("rot") && !line[0].contains("Frames")) {
                index++;
                vector<double> curve_points;
                for (int i = 1; i < line.size(); i++) {
                    double num = std::stod(line[i]);
                    curve_points.push_back(num);
                }
                curves.push_back(curve_points);
            }
        }
    } else {
        std::cerr << "Error opening file\n";
    }
    return curves;
}

void draw_curves(SDL_Renderer *rend, double *vals1, double *vals2) {
    vector<SDL_Rect> curve1;
    vector<SDL_Rect> curve2;
    auto result = run_example(vals1, vals2);
    int rect_size = 5;
    const int xOffset = 200;
    const int yOffset = HEIGHT / 2;
    for (int i = 0; i < n; i++) {
        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
        SDL_Rect rect1(xOffset + i * 50, yOffset - 50 * vals1[i], 10, 10);
        curve1.push_back(rect1);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect1.x + rect_size / 2, rect1.y + rect_size / 2, curve1[i - 1].x + rect_size / 2, curve1[i - 1].y + rect_size / 2);
        SDL_RenderDrawRect(rend, &rect1);
    }

    for (int i = 0; i < m; i++) {
        SDL_SetRenderDrawColor(rend, 0, 0, 200, 255);
        SDL_Rect rect2(xOffset + i * 50, yOffset + 150 - 50 * vals2[i], 10, 10);
        curve2.push_back(rect2);
        if (i > 0)
            SDL_RenderDrawLine(rend, rect2.x + rect_size / 2, rect2.y + rect_size / 2, curve2[i - 1].x + rect_size / 2, curve2[i - 1].y + rect_size / 2);
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
        SDL_RenderDrawLine(rend, curve1[first].x + rect_size / 2, curve1[first].y + rect_size / 2, curve2[second].x + rect_size / 2, curve2[second].y + rect_size / 2);
    }
}

void set_frames() {
    srand(time(NULL));
    for (int i = 0; i < 360; i++) {
        double *curve1 = (double *)(malloc(n * sizeof(double)));
        double *curve2 = (double *)(malloc(m * sizeof(double)));

        for (int i = 0; i < n; i++) {
            int deviation = rand() % 100;
            int v = vals1[i] + static_cast<double>(deviation / 50.0);
            curve1[i] = vals1[i] + static_cast<double>(deviation / 50.0);
        }

        for (int i = 0; i < m; i++) {
            int deviation = rand() % 100;
            int v = vals2[i] + static_cast<double>(deviation / 50.0);
            curve2[i] = vals2[i] + static_cast<double>(deviation / 50.0);
        }

        frames1.push_back(curve1);
        frames2.push_back(curve2);
    }
}

void display_text(SDL_Renderer* renderer) {
    // this opens a font style and sets a size
    TTF_Font *Sans = TTF_OpenFont("Sans.ttf", 24);

    // this is the color in rgb format,
    // maxing out all would give you the color white,
    // and it will be your text's color
    SDL_Color White = {255, 255, 255};

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface *surfaceMessage =
        TTF_RenderText_Solid(Sans, "put your text here", White);

    // now you can convert it into a texture
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect;  // create a rect
    Message_rect.x = 0;     // controls the rect's x coordinate
    Message_rect.y = 0;     // controls the rect's y coordinte
    Message_rect.w = 100;   // controls the width of the rect
    Message_rect.h = 100;   // controls the height of the rect

    // (0,0) is on the top left of the window/screen,
    // think a rect as the text's box,
    // that way it would be very simple to understand

    // Now since it's a texture, you have to put RenderCopy
    // in your game loop area, the area where the whole code executes

    // you put the renderer's name first, the Message,
    // the crop size (you can ignore this if you don't want
    // to dabble with cropping), and the rect which is the size
    // and coordinate of your texture
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    // Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void opengl_example() {
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

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

        // Draw a cube (die)
        glBegin(GL_QUADS);

        // Front face
        glColor3f(1.0, 0.0, 0.0);  // Red
        glVertex3f(-1.0, -1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);

        // Back face
        glColor3f(0.0, 1.0, 0.0);  // Green
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(1.0, -1.0, -1.0);

        // Left face
        glColor3f(0.0, 0.0, 1.0);  // Blue
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f(-1.0, -1.0, 1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(-1.0, 1.0, -1.0);

        // Right face
        glColor3f(1.0, 1.0, 0.0);  // Yellow
        glVertex3f(1.0, -1.0, -1.0);
        glVertex3f(1.0, 1.0, -1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, -1.0, 1.0);

        // Top face
        glColor3f(1.0, 0.0, 1.0);  // Magenta
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(-1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, 1.0);
        glVertex3f(1.0, 1.0, -1.0);

        // Bottom face
        glColor3f(0.0, 1.0, 1.0);  // Cyan
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