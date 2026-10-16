#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace gx::shader {
struct Semantic {
    uint32_t usage = 0, index = 0, reg = 0, mask = 15;
    uint32_t conversion = 0, componentType = 3;
};
struct InlineConstant { uint32_t index = 0, words[4]{}; };
struct TranslationState {
    uint32_t alphaFunc = 8, primitive = 4, clipPlanes = 0;
    bool alphaTest = false, fog = false;
    uint32_t fogMode = 0, samplerTypes[16]{};
    uint32_t shadowSamplers = 0;
};
struct TranslatedShader {
    std::vector<uint8_t> dxbc;
    std::vector<Semantic> inputs, outputs;
    std::vector<InlineConstant> inlineConstants[3];
    uint32_t floatWords = 0, intWords = 0, boolWords = 0;
};

// Converts actual legacy bytecode, not a same-name shader from a different game.
// Pixel conversion is keyed to its exact upstream vertex-output declaration.
// The caller owns caching by bytecode, vertex declaration and TranslationState.
bool TranslateLegacy(const void* code, size_t size, bool vertex,
                     const std::vector<Semantic>& declaration,
                     const TranslationState& state, TranslatedShader& output, std::string& error);
}
