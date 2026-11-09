#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace {
constexpr double PI = 3.14159265358979323846;

GLuint loadTexture(const std::string& path) {
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
    if (!pixels) {
        std::cerr << "Texture unavailable: " << path << " (" << stbi_failure_reason() << ")\\n";
        return 0;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    return texture;
}

void drawTexturedSphere(float x, float y, float radius, GLuint texture,
                        float rotation, float tint = 1.0f) {
    if (!texture) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(tint, tint, tint, 1.0f);
    constexpr int slices = 48;
    constexpr int stacks = 32;
    for (int stack = 0; stack < stacks; ++stack) {
        const float lat0 = static_cast<float>(-PI * 0.5 + PI * stack / stacks);
        const float lat1 = static_cast<float>(-PI * 0.5 + PI * (stack + 1) / stacks);
        glBegin(GL_QUAD_STRIP);
        for (int slice = 0; slice <= slices; ++slice) {
            const float u = static_cast<float>(slice) / slices;
            const float lon = u * static_cast<float>(2.0 * PI) + rotation;
            for (int row = 0; row < 2; ++row) {
                const float lat = row == 0 ? lat0 : lat1;
                const float nx = std::cos(lat) * std::cos(lon);
                const float ny = std::sin(lat);
                const float nz = std::cos(lat) * std::sin(lon);
                const float light = 0.22f + 0.78f * std::max(0.0f, nx * -0.42f + ny * 0.18f + nz * 0.89f);
                glColor4f(tint * light, tint * light, tint * light, 1.0f);
                glTexCoord2f(u, row == 0 ? static_cast<float>(stack) / stacks
                                          : static_cast<float>(stack + 1) / stacks);
                glVertex3f(x + radius * nx, y + radius * ny, radius * nz);
            }
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

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


const std::array<const char*, 7>& glyph(char c) {
    static const std::array<const char*, 7> blank = {"00000","00000","00000","00000","00000","00000","00000"};
    static const std::map<char, std::array<const char*, 7>> font = {
        {'A',{"01110","10001","10001","11111","10001","10001","10001"}},
        {'B',{"11110","10001","10001","11110","10001","10001","11110"}},
        {'C',{"01111","10000","10000","10000","10000","10000","01111"}},
        {'D',{"11110","10001","10001","10001","10001","10001","11110"}},
        {'E',{"11111","10000","10000","11110","10000","10000","11111"}},
        {'F',{"11111","10000","10000","11110","10000","10000","10000"}},
        {'G',{"01111","10000","10000","10111","10001","10001","01111"}},
        {'H',{"10001","10001","10001","11111","10001","10001","10001"}},
        {'I',{"11111","00100","00100","00100","00100","00100","11111"}},
        {'J',{"00111","00010","00010","00010","10010","10010","01100"}},
        {'K',{"10001","10010","10100","11000","10100","10010","10001"}},
        {'L',{"10000","10000","10000","10000","10000","10000","11111"}},
        {'M',{"10001","11011","10101","10101","10001","10001","10001"}},
        {'N',{"10001","11001","11001","10101","10011","10011","10001"}},
        {'O',{"01110","10001","10001","10001","10001","10001","01110"}},
        {'P',{"11110","10001","10001","11110","10000","10000","10000"}},
        {'Q',{"01110","10001","10001","10001","10101","10010","01101"}},
        {'R',{"11110","10001","10001","11110","10100","10010","10001"}},
        {'S',{"01111","10000","10000","01110","00001","00001","11110"}},
        {'T',{"11111","00100","00100","00100","00100","00100","00100"}},
        {'U',{"10001","10001","10001","10001","10001","10001","01110"}},
        {'V',{"10001","10001","10001","10001","10001","01010","00100"}},
        {'W',{"10001","10001","10001","10101","10101","11011","10001"}},
        {'X',{"10001","10001","01010","00100","01010","10001","10001"}},
        {'Y',{"10001","10001","01010","00100","00100","00100","00100"}},
        {'Z',{"11111","00001","00010","00100","01000","10000","11111"}},
        {'0',{"01110","10001","10011","10101","11001","10001","01110"}},
        {'1',{"00100","01100","00100","00100","00100","00100","01110"}},
        {'2',{"01110","10001","00001","00010","00100","01000","11111"}},
        {'3',{"11110","00001","00001","01110","00001","00001","11110"}},
        {'4',{"00010","00110","01010","10010","11111","00010","00010"}},
        {'5',{"11111","10000","10000","11110","00001","00001","11110"}},
        {'6',{"01110","10000","10000","11110","10001","10001","01110"}},
        {'7',{"11111","00001","00010","00100","01000","01000","01000"}},
        {'8',{"01110","10001","10001","01110","10001","10001","01110"}},
        {'9',{"01110","10001","10001","01111","00001","00001","01110"}},
        {'-',{"00000","00000","00000","11111","00000","00000","00000"}},
        {'*',{"00000","10101","01110","11111","01110","10101","00000"}},
        {'.',{"00000","00000","00000","00000","00000","00110","00110"}},
        {':',{"00000","00110","00110","00000","00110","00110","00000"}}
    };
    const auto it = font.find(c);
    return it == font.end() ? blank : it->second;
}

void drawText(const std::string& text, float x, float y, float scale,
              float red, float green, float blue, float alpha) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_QUADS);
    float cursor = x;
    for (char c : text) {
        const auto& rows = glyph(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (rows[row][col] != '1') continue;
                const float left = cursor + col * scale;
                const float top = y - row * scale;
                glVertex2f(left, top);
                glVertex2f(left + scale * 0.82f, top);
                glVertex2f(left + scale * 0.82f, top - scale * 0.82f);
                glVertex2f(left, top - scale * 0.82f);
            }
        }
        cursor += scale * 6.0f;
    }
    glEnd();
}

void drawLabel(const std::string& label, float x, float y, float scale,
               float alpha, bool bright = false) {
    drawText(label, x + 0.004f, y - 0.004f, scale, 0.12f, 0.20f, 0.42f, alpha * 0.45f);
    drawText(label, x, y, scale,
             bright ? 0.66f : 0.42f,
             bright ? 0.84f : 0.59f,
             bright ? 1.00f : 0.82f, alpha);
}

void drawSagittariusA(float time) {
    // A luminous, tilted accretion disk surrounds a truly dark event horizon.
    for (int layer = 0; layer < 7; ++layer) {
        const float rx = 0.155f + layer * 0.020f;
        const float ry = 0.040f + layer * 0.006f;
        glLineWidth(layer < 2 ? 5.0f : 2.0f);
        glColor4f(1.0f, 0.25f + layer * 0.075f, 0.035f, 0.10f + layer * 0.018f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 180; ++i) {
            const float a = static_cast<float>(2.0 * PI * i / 180);
            const float wobble = 1.0f + 0.055f * std::sin(3.0f * a + static_cast<float>(time * 0.35));
            const float xx = std::cos(a) * rx * wobble;
            const float yy = std::sin(a) * ry * wobble;
            const float tilt = -0.20f;
            glVertex2f(xx * std::cos(tilt) - yy * std::sin(tilt),
                       xx * std::sin(tilt) + yy * std::cos(tilt));
        }
        glEnd();
    }
    glLineWidth(1.0f);
    // Opaque event horizon cuts through the bright disk.
    glDisable(GL_BLEND);
    glColor4f(0.001f, 0.001f, 0.004f, 1.0f);
    circle(0.0f, 0.0f, 0.082f, 128);
    glEnable(GL_BLEND);
    glColor4f(1.0f, 0.78f, 0.38f, 0.72f);
    ring(0.0f, 0.0f, 0.091f, 1.0f, 0.76f, 0.42f, 0.72f);
    glColor4f(0.72f, 0.84f, 1.0f, 0.20f);
    ring(0.0f, 0.0f, 0.118f, 0.72f, 0.84f, 1.0f, 0.20f);
    drawLabel("SAGITTARIUS A*", -0.18f, 0.15f, 0.0060f, 0.98f, true);
}

void drawGalaxy(float time, float aspect) {
    // The arm structure rotates clockwise slowly around a stable galactic center.
    const float rotation = -time * 0.055f;
    const float maxRadius = 0.86f;
    const float galaxyXScale = std::max(1.0f, aspect * 0.98f);
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
        const float base = static_cast<float>(2.0 * PI * arm / arms) + static_cast<float>(PI * 0.25);
        for (int i = 0; i < 2400; ++i) {
            const float u = static_cast<float>(i) / 2399.0f;
            const float radius = 0.065f + u * (maxRadius - 0.065f);
            const float angle = base + std::log(radius / 0.075f) / pitch + rotation;
            const float scatter = std::sin(i * 12.9898f + arm * 78.233f) * 0.012f;
            const float r = radius + scatter;
            const float x = std::cos(angle) * r;
            const float y = std::sin(angle) * r;
            const float fade = 1.0f - u * 0.52f;
            const float dust = 0.5f + 0.5f * std::sin(i * 0.071f + arm);
            glColor4f(0.36f + 0.28f * dust, 0.43f + 0.25f * dust,
                      0.92f, 0.17f * fade);
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


    // The Orion Spur is a shorter local branch between the Sagittarius and Perseus arms.
    const float spurBase = 2.35f + rotation;
    std::vector<std::pair<float, float>> spurPoints;
    for (int i = 0; i <= 900; ++i) {
        const float u = static_cast<float>(i) / 900.0f;
        const float radius = 0.43f + u * 0.43f;
        const float angle = spurBase + std::log(radius / 0.66f) / pitch;
        const float x = std::cos(angle) * radius * galaxyXScale;
        const float y = std::sin(angle) * radius;
        spurPoints.emplace_back(x, y);
    }
    glowLine(spurPoints, 0.36f, 0.70f, 1.0f, 0.36f, 2.0f);
    drawLabel("ORION SPUR", -0.94f, 0.73f, 0.0070f, 0.96f, true);

    // Other major arms are identified quietly so they read as contextual cartography.
    drawLabel("PERSEUS ARM", 0.46f, 0.76f, 0.0058f, 0.34f);
    drawLabel("SAGITTARIUS ARM", -0.94f, -0.49f, 0.0058f, 0.31f);
    drawLabel("SCUTUM-CENTAURUS ARM", 0.42f, -0.78f, 0.0051f, 0.27f);
    drawLabel("NORMA ARM", 0.78f, 0.06f, 0.0058f, 0.27f);

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

void drawSunAndPlanets(double time, double speed, float aspect, const std::array<GLuint, 9>& textures) {
    // Keep the Sun physically attached to the Orion Spur: it shares the
    // galaxy's rotation and sits directly on the spur's centerline at r=0.66.
    // There is deliberately no separate circular orbit or detached golden trail.
    const float galaxyRotation = -static_cast<float>(time * 0.055);
    constexpr float sunRadius = 0.66f;
    const float spurAngle = 2.35f + galaxyRotation;
    const float sunX = std::cos(spurAngle) * sunRadius * std::max(1.0f, aspect * 0.98f);
    const float sunY = std::sin(spurAngle) * sunRadius;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    circle(sunX, sunY, 0.055f, 64);
    drawTexturedSphere(sunX, sunY, 0.040f, textures[0],
                       static_cast<float>(time * 0.035), 1.25f);
    drawLabel("SUN", sunX + 0.052f, sunY + 0.035f, 0.0055f, 0.88f, true);

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
        const float pr = p.radius * 2.2f;
        const double a = p.phase + time * p.speed * speed;
        const float eccentric = 1.0f - p.eccentricity * std::cos(static_cast<float>(a));
        const float orbitX = p.orbit * 1.25f * eccentric;
        const float orbitY = p.orbit * 1.25f * 0.82f * eccentric;
        const float px = sunX + std::cos(static_cast<float>(a)) * orbitX;
        const float py = sunY + std::sin(static_cast<float>(a)) * orbitY;

        // Draw only a faint orbital guide; the moving wake makes the Sun's pull legible.
        glColor4f(0.52f, 0.66f, 0.94f, 0.075f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 120; ++j) {
            const float oa = static_cast<float>(2.0 * PI * j / 120);
            const float e = 1.0f - p.eccentricity * std::cos(oa);
            glVertex2f(sunX + std::cos(oa) * p.orbit * 1.25f * e,
                       sunY + std::sin(oa) * p.orbit * 1.25f * 0.82f * e);
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
                sunX + std::cos(static_cast<float>(wa)) * p.orbit * 1.25f * we,
                sunY + std::sin(static_cast<float>(wa)) * p.orbit * 1.25f * 0.82f * we);
        }
        glowLine(planetWake, p.r, p.g, p.b, 0.42f, 1.2f);

        // Render mapped worlds as shaded 3-D spheres, not flat cartoon discs.
        const GLuint texture = textures[std::min<std::size_t>(index + 1, textures.size() - 1)];
        if (texture) {
            drawTexturedSphere(px, py, pr, texture,
                               static_cast<float>(time * p.speed * speed * 0.18), 1.0f);
        } else {
            glColor4f(p.r * 0.55f, p.g * 0.55f, p.b * 0.55f, 1.0f);
            circle(px, py, pr, 48);
        }
        // Distinctive rings for the gas/ice giants.
        if (p.rings) {
            glLineWidth(2.0f);
            glColor4f(p.r, p.g, p.b, 0.62f);
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < 64; ++j) {
                const float ra = static_cast<float>(2.0 * PI * j / 64);
                glVertex2f(px + std::cos(ra) * pr * 1.8f,
                           py + std::sin(ra) * pr * 0.62f);
            }
            glEnd();
            glLineWidth(1.0f);
        }
    }
}

void render(const std::vector<Star>& stars, int width, int height,
            double elapsed, double speed, const std::array<GLuint, 9>& textures) {
    const float aspect = static_cast<float>(width) / std::max(1, height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect, aspect, -1.0, 1.0, -2.0, 2.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.006f, 0.008f, 0.025f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_POINT_SMOOTH);

    drawStars(stars);
    drawGalaxy(static_cast<float>(elapsed), aspect);
    drawSagittariusA(static_cast<float>(elapsed));
    drawSunAndPlanets(elapsed, speed, aspect, textures);

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
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
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

    const std::array<std::string, 9> textureFiles = {
        "sun.jpg", "mercury.jpg", "venus.jpg", "earth.jpg", "mars.jpg",
        "jupiter.jpg", "saturn.jpg", "uranus.jpg", "neptune.jpg"
    };
    std::array<GLuint, 9> textures{};
    for (std::size_t i = 0; i < textureFiles.size(); ++i)
        textures[i] = loadTexture(std::string("assets/planet-textures/") + textureFiles[i]);

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
        render(stars, drawableW, drawableH, accumulated, speed, textures);
        SDL_GL_SwapWindow(window);
    }

    for (GLuint texture : textures) if (texture) glDeleteTextures(1, &texture);\n    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
