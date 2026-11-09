#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

namespace {
constexpr double PI = 3.14159265358979323846;

struct Star {
    float x, y, brightness, size;
};

struct Planet {
    float orbit;
    float speed;
    float phase;
    float radius;
    float r, g, b;
};

std::vector<Star> makeStars(int count, float xExtent, float yExtent, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> x(-xExtent, xExtent);
    std::uniform_real_distribution<float> y(-yExtent, yExtent);
    std::uniform_real_distribution<float> brightness(0.22f, 0.95f);
    std::uniform_real_distribution<float> size(1.0f, 2.4f);
    std::vector<Star> stars;
    stars.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i)
        stars.push_back({x(rng), y(rng), brightness(rng), size(rng)});
    return stars;
}

void circle(float x, float y, float radius, int segments = 48) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; ++i) {
        const float a = static_cast<float>(2.0 * PI * i / segments);
        glVertex2f(x + std::cos(a) * radius, y + std::sin(a) * radius);
    }
    glEnd();
}

void ring(float x, float y, float radius, float red, float green, float blue, float alpha) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 96; ++i) {
        const float a = static_cast<float>(2.0 * PI * i / 96);
        glVertex2f(x + std::cos(a) * radius, y + std::sin(a) * radius);
    }
    glEnd();
}

void glowLine(const std::vector<std::pair<float, float>>& points,
              float red, float green, float blue, float alpha, float width) {
    if (points.size() < 2) return;
    glLineWidth(width * 4.0f);
    glColor4f(red, green, blue, alpha * 0.12f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : points) glVertex2f(p.first, p.second);
    glEnd();

    glLineWidth(width * 2.0f);
    glColor4f(red, green, blue, alpha * 0.32f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : points) glVertex2f(p.first, p.second);
    glEnd();

    glLineWidth(width);
    glColor4f(red, green, blue, alpha);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : points) glVertex2f(p.first, p.second);
    glEnd();
}

void drawStars(const std::vector<Star>& stars) {
    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for (const auto& s : stars) {
        glColor4f(0.66f * s.brightness, 0.76f * s.brightness,
                  s.brightness, s.brightness);
        glVertex2f(s.x, s.y);
    }
    glEnd();
}

void drawGalaxy(float time, float aspect) {
    // The arm structure rotates clockwise slowly around a stable galactic center.
    const float rotation = -time * 0.018f;
    const float maxRadius = 0.78f;
    const float galaxyXScale = std::max(1.0f, aspect * 0.92f);
    const float pitch = 0.31f;
    const int arms = 4;

    // Soft galactic core.
    for (int i = 18; i >= 1; --i) {
        const float r = 0.018f * static_cast<float>(i);
        const float alpha = 0.010f + 0.0025f * static_cast<float>(19 - i);
        glColor4f(0.72f, 0.62f, 1.0f, alpha);
        circle(0.0f, 0.0f, r, 64);
    }

    // Dusty spiral arms, built from layered points. The arm rotation is intentionally subtle.
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int arm = 0; arm < arms; ++arm) {
        const float base = static_cast<float>(2.0 * PI * arm / arms);
        for (int i = 0; i < 2400; ++i) {
            const float u = static_cast<float>(i) / 2399.0f;
            const float radius = 0.075f + u * (maxRadius - 0.075f);
            const float angle = base + std::log(radius / 0.075f) / pitch + rotation;
            const float scatter = std::sin(i * 12.9898f + arm * 78.233f) * 0.012f;
            const float r = radius + scatter;
            const float x = std::cos(angle) * r;
            const float y = std::sin(angle) * r;
            const float fade = 1.0f - u * 0.52f;
            const float dust = 0.5f + 0.5f * std::sin(i * 0.071f + arm);
            glColor4f(0.36f + 0.28f * dust, 0.43f + 0.25f * dust,
                      0.92f, 0.08f * fade);
            glVertex2f(x * galaxyXScale, y);
        }
    }
    glEnd();

    // Bright star-forming knots along the arms.
    for (int arm = 0; arm < arms; ++arm) {
        const float base = static_cast<float>(2.0 * PI * arm / arms);
        for (int knot = 0; knot < 10; ++knot) {
            const float radius = 0.18f + knot * 0.057f;
            const float angle = base + std::log(radius / 0.075f) / pitch + rotation;
            const float x = std::cos(angle) * radius * galaxyXScale;
            const float y = std::sin(angle) * radius;
            glColor4f(0.45f, 0.58f, 1.0f, 0.055f);
            circle(x, y, 0.025f, 24);
            glColor4f(0.78f, 0.82f, 1.0f, 0.22f);
            circle(x, y, 0.004f, 12);
        }
    }

    // A restrained galactic boundary keeps the whole composition visually contained.
    glLineWidth(1.0f);
    glColor4f(0.32f, 0.38f, 0.72f, 0.10f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 128; ++i) {
        const float a = static_cast<float>(2.0 * PI * i / 128);
        glVertex2f(std::cos(a) * maxRadius * galaxyXScale, std::sin(a) * maxRadius);
    }
    glEnd();
}

void drawSunAndPlanets(double time, double speed, float aspect) {
    const double sunRate = 0.22 * speed;
    const double sunAngle = 2.35 - time * sunRate;
    constexpr float sunOrbit = 0.47f;
    const float sunOrbitX = sunOrbit * std::max(1.0f, aspect * 0.55f);
    const float sunX = std::cos(static_cast<float>(sunAngle)) * sunOrbitX;
    const float sunY = std::sin(static_cast<float>(sunAngle)) * sunOrbit;

    // The persistent golden wake marks the Sun's path around the galactic core.
    std::vector<std::pair<float, float>> trail;
    constexpr int trailSamples = 620;
    constexpr double trailSeconds = 32.0;
    trail.reserve(trailSamples);
    for (int i = 0; i < trailSamples; ++i) {
        const double fraction = static_cast<double>(i) / (trailSamples - 1);
        const double past = time - trailSeconds * (1.0 - fraction);
        const double a = 2.35 - past * sunRate;
        trail.emplace_back(std::cos(static_cast<float>(a)) * sunOrbitX,
                           std::sin(static_cast<float>(a)) * sunOrbit);
    }
    glowLine(trail, 1.0f, 0.58f, 0.16f, 0.96f, 3.0f);

    // A soft gravitational field: nested, subtly distorted contours around the Sun.
    for (int layer = 12; layer >= 1; --layer) {
        const float r = 0.055f + layer * 0.018f;
        glColor4f(1.0f, 0.55f, 0.16f, 0.010f + (12 - layer) * 0.0015f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 144; ++i) {
            const float a = static_cast<float>(2.0 * PI * i / 144);
            const float warp = 1.0f + 0.075f * std::sin(3.0f * a + static_cast<float>(time * 0.12));
            glVertex2f(sunX + std::cos(a) * r * warp,
                       sunY + std::sin(a) * r * warp * 0.78f);
        }
        glEnd();
    }

    // Layered corona and bright photosphere make the Sun the visual anchor.
    for (int i = 16; i >= 1; --i) {
        const float r = 0.012f + i * 0.0065f;
        glColor4f(1.0f, 0.34f + 0.025f * i, 0.06f, 0.012f);
        circle(sunX, sunY, r, 64);
    }
    glColor4f(1.0f, 0.58f, 0.12f, 0.42f);
    circle(sunX, sunY, 0.045f, 64);
    glColor4f(1.0f, 0.76f, 0.30f, 0.90f);
    circle(sunX, sunY, 0.030f, 64);
    glColor4f(1.0f, 0.91f, 0.61f, 1.0f);
    circle(sunX, sunY, 0.021f, 64);
    glColor4f(1.0f, 0.99f, 0.88f, 1.0f);
    circle(sunX - 0.004f, sunY + 0.005f, 0.010f, 48);

    struct PlanetData {
        float orbit, eccentricity, speed, phase, radius;
        float r, g, b;
        bool rings;
    };
    const PlanetData planets[] = {
        {0.060f, 0.08f, 2.75f, 0.4f, 0.0080f, 0.72f, 0.78f, 0.86f, false},
        {0.088f, 0.05f, 1.95f, 2.1f, 0.0100f, 0.94f, 0.62f, 0.34f, false},
        {0.122f, 0.07f, 1.42f, 4.2f, 0.0120f, 0.30f, 0.68f, 1.00f, false},
        {0.162f, 0.10f, 1.02f, 5.1f, 0.0105f, 0.88f, 0.38f, 0.27f, false},
        {0.218f, 0.06f, 0.61f, 1.4f, 0.0180f, 0.90f, 0.72f, 0.48f, true},
        {0.278f, 0.08f, 0.39f, 3.3f, 0.0150f, 0.76f, 0.68f, 0.54f, true},
        {0.335f, 0.05f, 0.27f, 0.8f, 0.0130f, 0.45f, 0.82f, 0.88f, true},
        {0.385f, 0.04f, 0.20f, 2.8f, 0.0125f, 0.32f, 0.48f, 0.90f, false}
    };

    for (std::size_t index = 0; index < std::size(planets); ++index) {
        const auto& p = planets[index];
        const double a = p.phase + time * p.speed * speed;
        const float eccentric = 1.0f - p.eccentricity * std::cos(static_cast<float>(a));
        const float orbitX = p.orbit * eccentric;
        const float orbitY = p.orbit * 0.82f * eccentric;
        const float px = sunX + std::cos(static_cast<float>(a)) * orbitX;
        const float py = sunY + std::sin(static_cast<float>(a)) * orbitY;

        // Draw only a faint orbital guide; the moving wake makes the Sun's pull legible.
        glColor4f(0.52f, 0.66f, 0.94f, 0.075f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 120; ++j) {
            const float oa = static_cast<float>(2.0 * PI * j / 120);
            const float e = 1.0f - p.eccentricity * std::cos(oa);
            glVertex2f(sunX + std::cos(oa) * p.orbit * e,
                       sunY + std::sin(oa) * p.orbit * 0.82f * e);
        }
        glEnd();

        // A short luminous orbital wake trails each world, curving around the Sun.
        std::vector<std::pair<float, float>> planetWake;
        constexpr int wakeSamples = 44;
        planetWake.reserve(wakeSamples);
        for (int j = 0; j < wakeSamples; ++j) {
            const double fraction = static_cast<double>(j) / (wakeSamples - 1);
            const double wa = a - (1.15 * (1.0 - fraction));
            const float we = 1.0f - p.eccentricity * std::cos(static_cast<float>(wa));
            planetWake.emplace_back(
                sunX + std::cos(static_cast<float>(wa)) * p.orbit * we,
                sunY + std::sin(static_cast<float>(wa)) * p.orbit * 0.82f * we);
        }
        glowLine(planetWake, p.r, p.g, p.b, 0.42f, 1.2f);

        // Planet halo, shaded disc, and a small illuminated crescent.
        glColor4f(p.r, p.g, p.b, 0.14f);
        circle(px, py, p.radius * 2.5f, 32);
        glColor4f(p.r * 0.42f, p.g * 0.42f, p.b * 0.48f, 1.0f);
        circle(px, py, p.radius, 40);
        glColor4f(p.r, p.g, p.b, 1.0f);
        circle(px - p.radius * 0.18f, py + p.radius * 0.16f,
               p.radius * 0.78f, 36);
        glColor4f(1.0f, 0.96f, 0.84f, 0.78f);
        circle(px - p.radius * 0.32f, py + p.radius * 0.34f,
               p.radius * 0.22f, 20);

        // Distinctive rings for the gas/ice giants.
        if (p.rings) {
            glLineWidth(2.0f);
            glColor4f(p.r, p.g, p.b, 0.62f);
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < 64; ++j) {
                const float ra = static_cast<float>(2.0 * PI * j / 64);
                glVertex2f(px + std::cos(ra) * p.radius * 1.8f,
                           py + std::sin(ra) * p.radius * 0.62f);
            }
            glEnd();
            glLineWidth(1.0f);
        }
    }
}

void render(const std::vector<Star>& stars, int width, int height,
            double elapsed, double speed) {
    const float aspect = static_cast<float>(width) / std::max(1, height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.006f, 0.008f, 0.025f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);

    drawStars(stars);
    drawGalaxy(static_cast<float>(elapsed), aspect);
    drawSunAndPlanets(elapsed, speed, aspect);

    glDisable(GL_BLEND);
}

struct Options {
    bool windowed = false;
    bool span = true;
    double speed = 1.0;
};

Options parseArgs(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--windowed") {
            options.windowed = true;
            options.span = false;
        } else if (arg == "--windowed-span") {
            options.windowed = true;
            options.span = true;
        } else if (arg == "--speed" && i + 1 < argc) {
            options.speed = std::clamp(std::atof(argv[++i]), 0.05, 8.0);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: galacticorbit-screensaver [--windowed|--windowed-span] [--speed N]\n";
            std::exit(0);
        }
    }
    return options;
}
} // namespace

int main(int argc, char** argv) {
    const Options options = parseArgs(argc, argv);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

    int x = SDL_WINDOWPOS_CENTERED;
    int y = SDL_WINDOWPOS_CENTERED;
    int width = 1280;
    int height = 720;
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (options.span) {
        int minX = 0, minY = 0, maxX = 0, maxY = 0;
        int displayCount = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
        for (int i = 0; displays && i < displayCount; ++i) {
            SDL_Rect bounds{};
            if (SDL_GetDisplayBounds(displays[i], &bounds)) {
                minX = std::min(minX, bounds.x);
                minY = std::min(minY, bounds.y);
                maxX = std::max(maxX, bounds.x + bounds.w);
                maxY = std::max(maxY, bounds.y + bounds.h);
            }
        }
        SDL_free(displays);
        x = minX;
        y = minY;
        width = std::max(1280, maxX - minX);
        height = std::max(720, maxY - minY);
        flags |= SDL_WINDOW_BORDERLESS;
    }

    // Prefer one borderless surface spanning the virtual desktop. Fullscreen
    // is reserved for the single-display fallback when span mode is disabled.
    if (!options.windowed && !options.span) flags |= SDL_WINDOW_FULLSCREEN;

    SDL_Window* window = SDL_CreateWindow("Galactic Orbit Screensaver",
        width, height, flags);
    if (window && options.span) SDL_SetWindowPosition(window, x, y);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_GL_SetSwapInterval(1);

    int drawableW = 0, drawableH = 0;
    SDL_GetWindowSizeInPixels(window, &drawableW, &drawableH);
    const auto stars = makeStars(2600,
        static_cast<float>(drawableW) / std::max(1, drawableH) * 1.15f,
        1.15f, 20261109u);

    bool running = true;
    bool paused = false;
    double speed = options.speed;
    double accumulated = 0.0;
    Uint64 previous = SDL_GetPerformanceCounter();
    const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                    case SDLK_Q: running = false; break;
                    case SDLK_SPACE: paused = !paused; break;
                    case SDLK_R: accumulated = 0.0; break;
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                    case SDLK_KP_PLUS: speed = std::min(8.0, speed * 1.2); break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS: speed = std::max(0.05, speed / 1.2); break;
                    case SDLK_F:
                        SDL_SetWindowFullscreen(window,
                            (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == 0);
                        break;
                    default: break;
                }
            }
        }

        const Uint64 now = SDL_GetPerformanceCounter();
        const double delta = static_cast<double>(now - previous) / frequency;
        previous = now;
        if (!paused) accumulated += std::min(delta, 0.1);

        SDL_GetWindowSizeInPixels(window, &drawableW, &drawableH);
        render(stars, drawableW, drawableH, accumulated, speed);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
