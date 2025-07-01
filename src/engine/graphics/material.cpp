#include "material.hpp"

namespace Engine::Graphics {
    Material::Material(Texture* texture, Shader* shader) : texture(texture), shader(shader) {}

    void Material::SetUniform(const std::string& name, const UniformValue& value) {
        mUniforms[name] = value;
    }

    void Material::Bind() const {
        if(!shader) return;
        
        shader->Bind();

        // Bind la couleur
        shader->SetVec4("u_Color", color);

        // TODO : upgrade to handle multiple textures ? And map their names to uniforms ?
        if(texture && texture->Get()) {
            texture->Bind(0);
            shader->SetBool("u_UseTexture", true);
            shader->SetInt("u_Texture", 0);
        }

        // Parcours les uniforms stockés dans le material, et les envoie au shader
        for (const auto& [name, value] : mUniforms) {
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>)         shader->SetInt(name, val);
                else if constexpr (std::is_same_v<T, float>)  shader->SetFloat(name, val);
                else if constexpr (std::is_same_v<T, bool>)   shader->SetBool(name, val);
                else if constexpr (std::is_same_v<T, glm::vec2>) shader->SetVec2(name, val);
                else if constexpr (std::is_same_v<T, glm::vec3>) shader->SetVec3(name, val);
                else if constexpr (std::is_same_v<T, glm::vec4>) shader->SetVec4(name, val);
                else if constexpr (std::is_same_v<T, glm::mat4>) shader->SetMat4(name, val);
            }, value);
        }
    }
}