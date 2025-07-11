#include "physicsystem.hpp"

using namespace Engine::Scene;

#include <iostream>
#include <random>
#include <chrono>
#include <iostream>
using namespace std::chrono;

namespace Engine::Physics {
    void PhysicSystem::OnFixedUpdate(float dt) {
        auto start = high_resolution_clock::now();

        ApplyMotion(dt);
        ResolveCollisions(dt);

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;
        physicsTime = elapsed.count() ;
    }

    void PhysicSystem::ApplyMotion(float dt) {
        for(auto entityID : GetRegistry().GetEntityIDsWith<Transform, Rigidbody>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& rigidbody = GetRegistry().GetComponent<Rigidbody>(entityID);
            
            if(!(transform.enabled && rigidbody.enabled)) continue;

            if(rigidbody.isKinematic || rigidbody.isSleeping) continue;

            if(rigidbody.isAffectedByGravity && !rigidbody.onGround) rigidbody.acceleration -= glm::vec3(gravity, 0.0f);

            rigidbody.velocity += (rigidbody.acceleration) * dt;           
            transform.Translate(rigidbody.velocity * PHYSICS_UNITS_PER_METER * dt);

            rigidbody.acceleration = glm::vec3(0.0f);
            rigidbody.velocity *= PHYSICS_DAMPING_FACTOR;

            // Gestion du sleep
            float velocitySquared = glm::length(rigidbody.velocity);
            if(velocitySquared < PHYSICS_SLEEP_SPEED_THRESHOLD) {
                rigidbody.sleepTimer += dt;
                if(rigidbody.sleepTimer >= PHYSICS_SLEEP_TIME_THREHSOLD) {
                    rigidbody.isSleeping = true;
                    rigidbody.velocity = glm::vec3(0.0f);
                }
            } else {
                rigidbody.isSleeping = false;
                rigidbody.sleepTimer = 0.0f;
            }
        }
    }

    void PhysicSystem::ResolveCollisions(float dt) {
        auto collidableIDs = GetRegistry().GetEntityIDsWith<Transform, Rigidbody, BoxCollider>();

        // Randomise l'ordre des entités pour créer un système moins biaisé
        std::shuffle(
            collidableIDs.begin(), 
            collidableIDs.end(), 
            std::mt19937{std::random_device{}()}
        );

        // Reset les flags
        for(auto entityID : collidableIDs) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& rb = GetRegistry().GetComponent<Rigidbody>(entityID);
            auto& collider = GetRegistry().GetComponent<BoxCollider>(entityID);

            if(!(transform.enabled && rb.enabled && collider.enabled)) continue;
    
            for (auto& [id, record] : collider.collisionsList) {
                record.updatedThisFrame = false;
            }
            for (auto& [id, record] : collider.triggersList) {
                record.updatedThisFrame = false;
            }

            rb.onGround = false;
            rb.onWall = false;
            // Mise à jour du collider
            collider.aabb = AABB(glm::vec2(transform.position), glm::vec2(collider.size * transform.scale), collider.enableRotation ? transform.rotation : glm::quat());
        }

        SpatialHash hash = BuildSpatialHash(collidableIDs);
        std::vector<std::pair<EntityID, EntityID>> candidates = GenerateBroadPhasePairs(hash);

        // LOG_DEBUG(std::string("NB COLLIDABLES " + collidableIDs.size()));
        // LOG_DEBUG(std::string("NB PAIRS " + candidates.size()));
        if(physicsTime > dt && maxIterations > 1) {
            maxIterations--;
        } else if (physicsTime <= dt && maxIterations < MAX_PHYSICS_ITERATIONS) {
            maxIterations++;
        }

        for(int iteration = 0; iteration < maxIterations; ++iteration) {
            bool collisionOccurred = false;

            for(auto [aID, bID] : candidates) {
                auto& ta = GetRegistry().GetComponent<Transform>(aID);
                auto& tb = GetRegistry().GetComponent<Transform>(bID);
                auto& ra = GetRegistry().GetComponent<Rigidbody>(aID);
                auto& rb = GetRegistry().GetComponent<Rigidbody>(bID);
                auto& ca = GetRegistry().GetComponent<BoxCollider>(aID);
                auto& cb = GetRegistry().GetComponent<BoxCollider>(bID);

                CollisionManifold manifold = CheckAABBCollision(ca.aabb, cb.aabb);

                if(!manifold.colliding || (ra.isKinematic && rb.isKinematic)) continue;

                // Liste de collisionsRecords en fonction des types (isTrigger ou pas)
                bool isTrigger = ca.isTrigger || cb.isTrigger;
                auto& aRecords = isTrigger ? ca.triggersList : ca.collisionsList;
                auto& bRecords = isTrigger ? cb.triggersList : cb.collisionsList;

                // Mise à jour des collider records
                // A => B
                if(aRecords.contains(bID)) {
                    ColliderRecord& record = aRecords[bID];
                    record.duration += dt;
                    record.updatedThisFrame = true;

                    if (GetRegistry().HasComponent<Behaviour>(aID)) {
                        auto scripts = GetRegistry().GetComponents<Behaviour>(aID);
                        for (auto script : scripts) {
                            if (isTrigger) script->OnTriggerStay(cb.GetEntity(), manifold);
                            else script->OnCollisionStay(cb.GetEntity(), manifold);
                        }
                    }
                } else {
                    aRecords[bID] = {bID, 0.0f, isTrigger, true};

                    if (GetRegistry().HasComponent<Behaviour>(aID)) {
                        auto scripts = GetRegistry().GetComponents<Behaviour>(aID);
                        for (auto script : scripts) {
                            if (isTrigger) script->OnTriggerEnter(cb.GetEntity(), manifold);
                            else script->OnCollisionEnter(cb.GetEntity(), manifold);
                        }
                    }
                }

                // B => A
                if (bRecords.contains(aID)) {
                    ColliderRecord& record = bRecords[aID];
                    record.duration += dt;
                    record.updatedThisFrame = true;

                    if (GetRegistry().HasComponent<Behaviour>(bID)) {
                        auto scripts = GetRegistry().GetComponents<Behaviour>(bID);
                        for (auto script : scripts) {
                            if (isTrigger) script->OnTriggerStay(ca.GetEntity(), manifold);
                            else script->OnCollisionStay(ca.GetEntity(), manifold);
                        }
                    }
                } else {
                    bRecords[aID] = {aID, 0.0f, isTrigger, true};

                    if (GetRegistry().HasComponent<Behaviour>(bID)) {
                        auto scripts = GetRegistry().GetComponents<Behaviour>(bID);
                        for (auto script : scripts) {
                            if (isTrigger) script->OnTriggerEnter(ca.GetEntity(), manifold);
                            else script->OnCollisionEnter(ca.GetEntity(), manifold);
                        }
                    }
                }

                // Calcul de la vélocité selon la normale de la collision
                glm::vec3 relativeVelocity = rb.velocity - ra.velocity;
                float velAlongNormal = glm::dot(relativeVelocity, manifold.normal);

                // Réveille les entités endormies en cas de collision
                // Check si la vitesse projetée (ou la vitesse de l'objet qui entre en collision) est assez élevée pour wake up
                if (ra.isSleeping && !ra.isKinematic) {
                    if(std::abs(velAlongNormal) > PHYSICS_SLEEP_SPEED_THRESHOLD || glm::length(rb.velocity) > PHYSICS_SLEEP_SPEED_THRESHOLD || std::abs(manifold.penetration) > PHYSICS_SLEEP_SPEED_THRESHOLD) {
                        ra.isSleeping = false;
                        ra.sleepTimer = 0.0f;
                    }
                }
                
                if (rb.isSleeping && !rb.isKinematic) {
                    if(std::abs(velAlongNormal) > PHYSICS_SLEEP_SPEED_THRESHOLD || glm::length(ra.velocity) > PHYSICS_SLEEP_SPEED_THRESHOLD  || std::abs(manifold.penetration) > PHYSICS_SLEEP_SPEED_THRESHOLD) {
                        rb.isSleeping = false;
                        rb.sleepTimer = 0.0f;
                    }
                }

                // Skip la résolution entre deux objets endormis (ou si l'un des deux est un trigger)
                if ((ra.isSleeping && rb.isSleeping) || isTrigger) continue;

                // Correction de pénétration
                float correctionFactor = 0.8f;
                glm::vec3 correction = manifold.normal * manifold.penetration * correctionFactor;

                float invMassA = (ra.isKinematic || ra.mass <= 0) ? 0.0f : 1.0f / ra.mass;
                float invMassB = (rb.isKinematic || rb.mass <= 0) ? 0.0f : 1.0f / rb.mass;
                float invMassSum = invMassA + invMassB;

                if (invMassSum > 0.0f) {
                    glm::vec3 moveA = -correction * (invMassA / invMassSum);
                    glm::vec3 moveB =  correction * (invMassB / invMassSum);

                    if (!ra.isKinematic) ta.position += moveA;
                    if (!rb.isKinematic) tb.position += moveB;
                }

                // Gestion du rebond si au moins un est bounceable
                if (ra.isBounceable || rb.isBounceable) {
                    if (velAlongNormal < 0.0f) {
                        float restitution = 0.5f * (ra.restitution + rb.restitution);

                        float impulseMag = -(1.0f + restitution) * velAlongNormal / invMassSum;
                        glm::vec3 impulse = impulseMag * manifold.normal;

                        if (!ra.isKinematic) ra.velocity -= impulse * invMassA;
                        if (!rb.isKinematic) rb.velocity += impulse * invMassB;
                    }
                }

                // Gestion du ralentissement dû a la friction contre un objet
                // Appliquer la friction dynamique sur chaque objet
                auto applyFriction = [](Rigidbody& rb, const glm::vec2& normal, float friction) {
                    glm::vec3 tangent = glm::normalize(glm::vec3(-normal.y, normal.x, 0.0f));

                    float tangentialSpeed = glm::dot(rb.velocity, tangent);
                    float frictionMag = tangentialSpeed * friction;

                    // Soustraire une portion de la vitesse tangentielle
                    if (std::abs(tangentialSpeed) > 0.01f) {
                        rb.velocity -= tangent * frictionMag;
                    }
                };

                float frictionA = ra.friction;
                float frictionB = rb.friction;
                float combinedFriction = (frictionA + frictionB) * 0.5f;

                // Attention à NaN
                if (std::isfinite(combinedFriction) && combinedFriction > 0.0f) {
                    if(!ra.isKinematic) applyFriction(ra, manifold.normal, combinedFriction);
                    if(!rb.isKinematic) applyFriction(rb, -manifold.normal, combinedFriction);
                }

                // Détection "au sol" et "contre un mur"
                if (!ra.isKinematic) {
                    if (glm::dot(manifold.normal, glm::vec3(0, 1, 0)) > 0.5f) ra.onGround = true;
                    if (std::abs(glm::dot(manifold.normal, glm::vec3(1, 0, 0))) > 0.5f) ra.onWall = true;
                }
                if (!rb.isKinematic) {
                    if (glm::dot(-manifold.normal, glm::vec3(0, 1, 0)) > 0.5f) rb.onGround = true;
                    if (std::abs(glm::dot(-manifold.normal, glm::vec3(1, 0, 0))) > 0.5f) rb.onWall = true;
                }

                collisionOccurred = true;
            }

            if(!collisionOccurred) break;
        }

        // Nettoyage des collider records et appel aux callbacks si on est sorti d'une collision pendant la frame
        for (auto entityID : collidableIDs) {
            auto& collider = GetRegistry().GetComponent<BoxCollider>(entityID);

            auto cleanupRecords = [&](auto& records, bool isTrigger) {
                std::vector<EntityID> toRemove;

                for (auto& [otherID, record] : records) {
                    if (!record.updatedThisFrame) {
                        record.duration += dt;

                        if (record.duration >= COLLISION_EXPIRE_THRESHOLD) {
                            toRemove.push_back(otherID);

                            if (GetRegistry().HasComponent<Behaviour>(entityID)) {
                                auto scripts = GetRegistry().GetComponents<Behaviour>(entityID);
                                auto& otherCollider = GetRegistry().GetComponent<BoxCollider>(otherID);
                                CollisionManifold dummyManifold{.colliding = false};

                                for (auto script : scripts) {
                                    if (isTrigger) script->OnTriggerExit(otherCollider.GetEntity(), dummyManifold);
                                    else script->OnCollisionExit(otherCollider.GetEntity(), dummyManifold);
                                }
                            }
                        }
                    }
                }

                for (auto id : toRemove) {
                    records.erase(id);
                }
            };

            cleanupRecords(collider.collisionsList, false);
            cleanupRecords(collider.triggersList, true);
        }
    }

    CollisionManifold PhysicSystem::CheckAABBCollision(const AABB& a, const AABB& b) {
        CollisionManifold manifold;
        manifold.colliding = false;

        glm::vec2 delta = b.center - a.center;
        glm::vec2 overlap = (a.halfSize + b.halfSize) - glm::abs(delta);
        
        const float EPSILON = 0.001f;
        if(overlap.x > EPSILON && overlap.y > EPSILON) {
            manifold.colliding = true;

            if(overlap.x < overlap.y) {
                manifold.penetration = overlap.x;
                manifold.normal = glm::vec3((delta.x > 0) ? 1.0f : -1.0f, 0.0f, 0.0f);
            } else {
                manifold.penetration = overlap.y;
                manifold.normal = glm::vec3(0.0f, (delta.y > 0) ? 1.0f : -1.0f, 0.0f);
            }
        }

        return manifold;
    }

    CollisionManifold PhysicSystem::CheckOBBCollision(const OBB& a, const OBB& b) {
        CollisionManifold manifold;
        manifold.colliding = false;

        const float EPSILON = 1e-6f;
        float minPenetration = FLT_MAX;
        glm::vec3 bestAxis;

        float R[3][3], AbsR[3][3];

        glm::vec3 t = b.center - a.center;
        t = glm::vec3(glm::dot(t, a.axes[0]), glm::dot(t, a.axes[1]), glm::dot(t, a.axes[2]));

        for(int i = 0; i < 3; ++i) {
            for(int j = 0; j < 3; ++j) {
                R[i][j] = glm::dot(a.axes[i], b.axes[j]);
                AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
            }
        }

        float ra, rb, penetration;
        glm::vec3 axis;

        // Tester axes A0, A1, A2
        for (int i = 0; i < 3; ++i) {
            ra = a.halfExtents[i];
            rb = b.halfExtents[0] * AbsR[i][0] + b.halfExtents[1] * AbsR[i][1] + b.halfExtents[2] * AbsR[i][2];
            float dist = std::abs(t[i]);

            penetration = (ra + rb) - dist;
            if (penetration < 0) return manifold; // séparation trouvée
            if (penetration < minPenetration) {
                minPenetration = penetration;
                axis = a.axes[i];
                // Sens selon le vecteur t (pour normal)
                if (t[i] < 0) axis = -axis;
            }
        }

        // Tester axes B0, B1, B2
        for (int i = 0; i < 3; ++i) {
            ra = a.halfExtents[0] * AbsR[0][i] + a.halfExtents[1] * AbsR[1][i] + a.halfExtents[2] * AbsR[2][i];
            rb = b.halfExtents[i];
            float dist = std::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]);

            penetration = (ra + rb) - dist;
            if (penetration < 0) return manifold; // séparation trouvée
            if (penetration < minPenetration) {
                minPenetration = penetration;
                axis = b.axes[i];
                float sign = glm::dot(b.axes[i], b.center - a.center) < 0 ? -1.0f : 1.0f;
                axis = sign * axis;
            }
        }

        // Tester axes croisements A_i x B_j
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (AbsR[i][j] < EPSILON) continue; // axes presque parallèles => pas de test

                ra = a.halfExtents[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + a.halfExtents[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
                rb = b.halfExtents[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + b.halfExtents[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

                float dist = std::abs(t[(i + 2) % 3] * R[(i + 1) % 3][j] - t[(i + 1) % 3] * R[(i + 2) % 3][j]);
                penetration = (ra + rb) - dist;

                if (penetration < 0) return manifold; // séparation trouvée

                if (penetration < minPenetration) {
                    minPenetration = penetration;

                    axis = glm::cross(a.axes[i], b.axes[j]);
                    axis = glm::normalize(axis);

                    // Sens de la normale selon le vecteur centre->centre
                    float sign = glm::dot(axis, b.center - a.center) < 0 ? -1.0f : 1.0f;
                    axis = sign * axis;
                }
            }
        }

        // Collision confirmée, remplir manifold
        manifold.colliding = true;
        manifold.normal = axis;
        manifold.penetration = minPenetration;
        return manifold;
    }

    SpatialHash PhysicSystem::BuildSpatialHash(const std::vector<EntityID>& collidableIDs) {
        SpatialHash output;
        for(EntityID id : collidableIDs) {
            auto& t = GetRegistry().GetComponent<Transform>(id);
            auto& c = GetRegistry().GetComponent<BoxCollider>(id);
            
            // Mapping de la taille de l'aabb du collider selon la taille des cellules
            // (Permet de ramener la position/taille du collider à un emplacement dans une ou plusieurs cellules de taille donnée)
            int minX = floor((c.aabb.center.x - c.aabb.halfSize.x)/SPATIAL_HASH_CELL_SIZE);
            int maxX = floor((c.aabb.center.x + c.aabb.halfSize.x)/SPATIAL_HASH_CELL_SIZE);
            int minY = floor((c.aabb.center.y - c.aabb.halfSize.y)/SPATIAL_HASH_CELL_SIZE);
            int maxY = floor((c.aabb.center.y + c.aabb.halfSize.y)/SPATIAL_HASH_CELL_SIZE);

            // Pour chaque indice de cellule (entre la plus petite et la plus élevée) dans laquelle notre entité rentre, l'y ajouter.
            for(int cx = minX; cx <= maxX; ++cx) {
                for(int cy = minY; cy <= maxY; ++cy) {
                    output[{(uint32_t)cx, (uint32_t)cy}].push_back(id);
                }
            }
        }

        return output;
    }

    std::vector<std::pair<EntityID, EntityID>> PhysicSystem::GenerateBroadPhasePairs(SpatialHash spatialHash) {
        std::set<std::pair<EntityID, EntityID>> pairSet;

        for(auto& bucket : spatialHash) {
            auto& vec = bucket.second;
            for(size_t i = 0; i < vec.size(); ++i) {
                for(size_t j = i + 1; j < vec.size(); ++j) {
                    pairSet.insert({std::min(vec[i], vec[j]), std::max(vec[i], vec[j])});
                }
            }
        }

        return {pairSet.begin(), pairSet.end()};
    }
}