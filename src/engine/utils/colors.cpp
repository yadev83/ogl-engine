#include "colors.hpp"

namespace Engine::Utils {
    glm::vec4 RGBAColor(int r, int g, int b, int alpha) {
        float validR = r > 255 ? 255.0f : r < 0 ? 0.0f : (float)r;
        float validG = g > 255 ? 255.0f : g < 0 ? 0.0f : (float)g;
        float validB = b > 255 ? 255.0f : b < 0 ? 0.0f : (float)b;
        float validA = alpha > 255 ? 255.0f : alpha < 0 ? 0.0f : (float)alpha;

        return glm::vec4(
            validR/255.0f,
            validG/255.0f,
            validB/255.0f,
            validA/255.0f
        );
    }
}