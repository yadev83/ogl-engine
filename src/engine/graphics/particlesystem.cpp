#include "particlesystem.hpp"

#include "../ecs/entity.hpp"
#include "../scene/transform.hpp"
#include "../utils/rng.hpp"
#include "sprite.hpp"

using namespace Engine::Scene;
namespace Engine::Graphics {
    void ParticleSystem::OnUpdate(float deltaTime) {
        // Update lifetime and remove dead particles
        for (auto entityID : GetRegistry().GetEntityIDsWith<Particle, Transform, Sprite>()) {
            auto& particle = GetRegistry().GetComponent<Particle>(entityID);
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& sprite = GetRegistry().GetComponent<Sprite>(entityID);

            if(!(particle.enabled && transform.enabled && sprite.enabled)) continue;

            particle.lifetime += deltaTime;
            if (particle.lifetime >= particle.maxLifetime) {
                GetRegistry().DestroyEntity(entityID);
                continue;
            }

            // Move particle
            transform.lastPosition = transform.position;
            transform.position += glm::vec3(particle.velocity) * deltaTime;

            // Optional: fade opacity, reduce size, etc.
            if(particle.fadeOut) {
                float transparency = std::clamp(particle.lifetime / particle.maxLifetime, 0.0f, particle.initialOpacity);
                sprite.material.color.a = particle.initialOpacity - transparency;
            }
        }

         // Emit particles from emitters
        for (auto entityID : GetRegistry().GetEntityIDsWith<ParticleEmitter, Transform>()) {
            auto& emitter = GetRegistry().GetComponent<ParticleEmitter>(entityID);
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);

            if(!(emitter.enabled && transform.enabled)) continue;
            if (!emitter.active) continue;

            emitter.timeSinceLastEmission += deltaTime;
            float interval = 1.0f / emitter.emissionRate;

            while (emitter.timeSinceLastEmission >= interval) {
                emitter.timeSinceLastEmission -= interval;

                // Create new particle
                if (GetRegistry().GetEntityIDsWith<Particle>().size() < emitter.maxParticles) {
                    ECS::Entity particle(GetRegistry().CreateEntity(), &GetRegistry());

                    Transform pTransform = transform; // start at emitter position
                    particle.AddComponent<Transform>(pTransform);

                    float initialSpeed = emitter.particleMaxInitialSpeed < emitter.particleMinInitialSpeed ? emitter.particleMaxInitialSpeed : 
                        emitter.particleMinInitialSpeed > emitter.particleMaxInitialSpeed ? emitter.particleMinInitialSpeed : 
                        Utils::RandomFloat(emitter.particleMinInitialSpeed, emitter.particleMaxInitialSpeed);

                    float initialSize = emitter.particleMaxInitialSize < emitter.particleMinInitialSize ? emitter.particleMaxInitialSize : 
                        emitter.particleMinInitialSize > emitter.particleMaxInitialSize ? emitter.particleMinInitialSize : 
                        Utils::RandomFloat(emitter.particleMinInitialSize, emitter.particleMaxInitialSize);

                    float initialOpacity = emitter.particleMaxInitialOpacity < emitter.particleMinInitialOpacity ? emitter.particleMaxInitialOpacity : 
                        emitter.particleMinInitialOpacity > emitter.particleMaxInitialOpacity ? emitter.particleMinInitialOpacity : 
                        Utils::RandomFloat(emitter.particleMinInitialOpacity, emitter.particleMaxInitialOpacity);

                    float lifetime = emitter.particleMaxLifetime < emitter.particleMinLifetime ? emitter.particleMaxLifetime : 
                        emitter.particleMinLifetime > emitter.particleMaxLifetime ? emitter.particleMinLifetime : 
                        Utils::RandomFloat(emitter.particleMinLifetime, emitter.particleMaxLifetime);

                    float randomAngle = Utils::RandomAngle();
                    glm::vec3 velocity = glm::vec3(cos(randomAngle), sin(randomAngle), 1.0f) * initialSpeed;
                    Particle p;
                    p.velocity = velocity;
                    p.maxLifetime = lifetime;
                    p.initialOpacity = initialOpacity;
                    p.fadeOut = emitter.fadeOut;

                    particle.AddComponent<Particle>(p);
                    auto& sprite = particle.AddComponent<Sprite>();
                    sprite.material = Material(emitter.sourceMaterial);
                    sprite.size = glm::vec3(initialSize);
                    sprite.material.color.a = p.initialOpacity;
                }
            }
        }
    }
}