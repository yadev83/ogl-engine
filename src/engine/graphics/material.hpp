/**
 * @file material.hpp
 * @brief Définit l'implémentation d'un Material
 */
#pragma once

#include "defaults.hpp"

#include "texture.hpp"
#include "shader.hpp"

#include <variant>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine::Graphics {
    /**
     * @brief Permet de ranger sous le nom "UniformValue" tous les types actuellement gérés par les uniforms shader
     * 
     */
    using UniformValue = std::variant<
        int,
        float,
        bool,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::mat4
    >;

    /**
     * @brief La classe Material définit un "material" graphique utilisé pour le rendu de meshes texturés.
     * 
     * Un Material doit nécessairement avoir un Shader associé pour le rendu. Il peut avoir une texture aussi.
     * En plus de cela, le Material possède une "color" (qui peut être utilisée pour un rendu flat si pas de texture ou en mix avec la texture)
     * 
     */
    class Material {
        private:
            /** @brief Liste d'uniforms à garder en mémoire pour les réappliquer entre plusieurs draw calls */
            std::unordered_map<std::string, UniformValue> mUniforms;

        public:
            /** @brief Pointeur vers un objet Shader préalablement compilé */
            Shader* shader;
            /** @brief Pointeur vers un objet Texture (peut être nullptr) */
            Texture* texture;
            /** @brief La couleur du Material (automatiquement envoyée à l'uniform u_Color, peut être utilisée par les shaders) */
            glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

            /**
             * @brief Construit un nouveau Material
             * 
             * @param texture La texture à utiliser (charger au préalable et gérer la mémoire)
             * @param shader Le shader à utiliser (charger au préalable, et gérer la mémoire)
             */
            Material(Texture* texture = Defaults::texture, Shader* shader = Defaults::shader);
            
            /**
             * @brief Enregistre un uniform dans le tableau interne du material
             * 
             * A chaque "Bind()", les uniforms stockés sont envoyés dans le Shader.
             * Cela permet de rendre les Shader particulièrement customisables, 
             * sans pour autant mettre les mains dans le code des renderers puisuqe le binding à lieu ici.
             * 
             * @param name Le nom de l'uniform (doit matcher avec le nom utilisé dans le shader)
             * @param value La valeur à enregistrer (doit faire partie de UniformValue)
             */
            void SetUniform(const std::string& name, const UniformValue& value);
            /**
             * @brief Permet de lier le Material courant et de setup les uniforms avant un drawcall
             */
            void Bind() const;
    };
}