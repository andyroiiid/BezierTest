#include <SDL.h>
#include <glm/glm.hpp>

using namespace glm;

struct bezier {
    vec2 p1{0};
    vec2 p2{0};
    vec2 p3{0};
    vec2 p4{0};

    [[nodiscard]] vec2 sample(float t) const {
        auto p12 = mix(p1, p2, t);
        auto p23 = mix(p2, p3, t);
        auto p34 = mix(p3, p4, t);
        auto p1223 = mix(p12, p23, t);
        auto p2334 = mix(p23, p34, t);
        return mix(p1223, p2334, t);
    }

    void divide(bezier &b1, bezier &b2) const {
        b1.p1 = p1;
        b1.p2 = (p1 + p2) * 0.5f;
        b1.p3 = (p1 + 2.0f * p2 + p3) * 0.25f;
        b1.p4 = (p1 + 3.0f * (p2 + p3) + p4) * 0.125f;

        b2.p1 = b1.p4;
        b2.p2 = (p2 + 2.0f * p3 + p4) * 0.25f;
        b2.p3 = (p3 + p4) * 0.5f;
        b2.p4 = p4;
    }

    void drawControlLine(SDL_Renderer *renderer) const {
        SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderDrawLineF(renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderDrawLineF(renderer, p3.x, p3.y, p4.x, p4.y);
    }

    void drawUniformSampled(SDL_Renderer *renderer, size_t samples) const {
        vec2 prev = p1;
        for (size_t i = 0; i < samples; i++) {
            vec2 curr = sample(float(i) / float(samples));
            SDL_RenderDrawLineF(renderer, prev.x, prev.y, curr.x, curr.y);
            prev = curr;
        }
        SDL_RenderDrawLineF(renderer, prev.x, prev.y, p4.x, p4.y);
    }

    void drawDivided(SDL_Renderer *renderer, int level) const {
        drawControlLine(renderer);
        if (level < 1) {
            return;
        }
        bezier b1, b2;
        divide(b1, b2);
        b1.drawDivided(renderer, level - 1);
        b2.drawDivided(renderer, level - 1);
    }
};

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("bezier", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    bezier test;
    test.p1 = {150.0f, 450.0f};
    test.p2 = {250.0f, 100.0f};
    test.p3 = {350.0f, 450.0f};
    test.p4 = {450.0f, 100.0f};

    int currPoint = 0;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_1:
                            currPoint = 1;
                            break;
                        case SDL_SCANCODE_2:
                            currPoint = 2;
                            break;
                        case SDL_SCANCODE_3:
                            currPoint = 3;
                            break;
                        case SDL_SCANCODE_4:
                            currPoint = 4;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        currPoint = 0;
                        SDL_Log("p1 %f, %f p2 %f, %f p3 %f, %f p4 %f %f",
                                test.p1.x, test.p1.y,
                                test.p2.x, test.p2.y,
                                test.p3.x, test.p3.y,
                                test.p4.x, test.p4.y);
                    }
                    break;
                default:
                    break;
            }
        }

        int x, y;
        SDL_GetMouseState(&x, &y);
        switch (currPoint) {
            case 1:
                test.p1 = vec2(x, y);
                break;
            case 2:
                test.p2 = vec2(x, y);
                break;
            case 3:
                test.p3 = vec2(x, y);
                break;
            case 4:
                test.p4 = vec2(x, y);
                break;
            default:
                break;
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        test.drawControlLine(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        test.drawUniformSampled(renderer, 4);

        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        test.drawUniformSampled(renderer, 8);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        test.drawUniformSampled(renderer, 16);

//        test.drawDivided(renderer, 5);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
