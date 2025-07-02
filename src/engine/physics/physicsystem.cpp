#include "physicsystem.hpp"

using namespace Engine::Scene;

#include <iostream>
#include <random>

namespace Engine::Physics {
    void PhysicSystem::OnFixedUpdate(float dt) {
        ApplyMotion(dt);
        ResolveCollisions(dt);
    }

    void PhysicSystem::ApplyMotion(float dt) {
        for(auto entityID : GetRegistry().GetEntityIDsWith<Transform, Rigidbody>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& rigidbody = GetRegistry().GetComponent<Rigidbody>(entityID);

            if(rigidbody.isKinematic) {
                rigidbody.isSleeping = true;
            }

            if(rigidbody.isSleeping) continue;

            if(rigidbody.isAffectedByGravity && !rigidbody.onGround) rigidbody.acceleration -= glm::vec3(gravity, 0.0f);

            rigidbody.velocity += (rigidbody.acceleration) * dt;           
            transform.Translate(rigidbody.velocity * unitsPerMeter * dt);

            rigidbody.acceleration = glm::vec3(0.0f);
            rigidbody.velocity *= dampingFactor;

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
            auto& rb = GetRegistry().GetComponent<Rigidbody>(entityID);
            rb.onGround = false;
            rb.onWall = false;
        }

        for(int iteration = 0; iteration < MAX_PHYSICS_ITERATIONS; ++iteration) {
            bool collisionOccurred = false;

            for(size_t i = 0; i < collidableIDs.size(); ++i) {
                for(size_t j = i + 1; j < collidableIDs.size(); ++j) {
                    EntityID aID = collidableIDs[i];
                    EntityID bID = collidableIDs[j];

                    auto& ta = GetRegistry().GetComponent<Transform>(aID);
                    auto& tb = GetRegistry().GetComponent<Transform>(bID);
                    auto& ra = GetRegistry().GetComponent<Rigidbody>(aID);
                    auto& rb = GetRegistry().GetComponent<Rigidbody>(bID);
                    auto& ca = GetRegistry().GetComponent<BoxCollider>(aID);
                    auto& cb = GetRegistry().GetComponent<BoxCollider>(bID);

                    AABB aBox(glm::vec2(ta.position), glm::vec2(ca.size * ta.scale), ta.rotation);
                    AABB bBox(glm::vec2(tb.position), glm::vec2(cb.size * tb.scale), tb.rotation);

                    CollisionManifold manifold = CheckAABBCollision(aBox, bBox);

                    // Liste de collisionsRecords en fonction des types (isTrigger ou pas)
                    bool isTrigger = ca.isTrigger || cb.isTrigger;
                    auto& aRecords = isTrigger ? ca.triggersList : ca.collisionsList;
                    auto& bRecords = isTrigger ? ca.triggersList : ca.collisionsList;

                    // Si pas de collision, on se débarasse des collisionsRecords et on appelle les callbacks s'il y en a, puis on skip le reste
                    if (!manifold.colliding) {
                        if(aRecords.contains(bID) && aRecords[bID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            aRecords.erase(bID);

                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& aScript = GetRegistry().GetComponent<Behaviour>(aID);
                                if(isTrigger) aScript.OnTriggerExit(cb.GetEntity(), manifold);
                                else aScript.OnCollisionExit(cb.GetEntity(), manifold);
                            }
                        }

                        if(bRecords.contains(bID) && bRecords[bID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            bRecords.erase(bID);

                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& bScript = GetRegistry().GetComponent<Behaviour>(bID);
                                if(isTrigger) bScript.OnTriggerExit(ca.GetEntity(), manifold);
                                else bScript.OnCollisionExit(ca.GetEntity(), manifold);
                            }
                        }
                        
                        continue;
                    }

                    // En cas de collision, on insert ou update les collisionsRecords et appelle les callbacks s'il y en a avant de passer à la suite
                    if(aRecords.contains(bID)) {
                        aRecords[bID].duration += dt;

                        if(GetRegistry().HasComponent<Behaviour>(aID)) {
                            auto& aScript = GetRegistry().GetComponent<Behaviour>(aID);
                            if(isTrigger) aScript.OnTriggerStay(cb.GetEntity(), manifold);
                            else aScript.OnCollisionStay(cb.GetEntity(), manifold);
                        }
                    } else {
                        aRecords[bID] = {bID, 0.0f, isTrigger};

                        if(GetRegistry().HasComponent<Behaviour>(aID)) {
                            auto& aScript = GetRegistry().GetComponent<Behaviour>(aID);
                            if(isTrigger) aScript.OnTriggerEnter(cb.GetEntity(), manifold);
                            else aScript.OnCollisionEnter(cb.GetEntity(), manifold);
                        }
                    }

                    if(bRecords.contains(bID)) {
                        bRecords[aID].duration += dt;

                        if(GetRegistry().HasComponent<Behaviour>(bID)) {
                            auto& bScript = GetRegistry().GetComponent<Behaviour>(bID);
                            if(isTrigger) bScript.OnTriggerStay(ca.GetEntity(), manifold);
                            else bScript.OnCollisionStay(ca.GetEntity(), manifold);
                        }
                    } else {
                        bRecords[aID] = {aID, 0.0f, isTrigger};

                        if(GetRegistry().HasComponent<Behaviour>(bID)) {
                            auto& bScript = GetRegistry().GetComponent<Behaviour>(bID);
                            if(isTrigger) bScript.OnTriggerEnter(ca.GetEntity(), manifold);
                            else bScript.OnCollisionEnter(ca.GetEntity(), manifold);
                        }
                    }

                    // Calcul de la vélocité selon la normale de la collision
                    glm::vec3 relativeVelocity = rb.velocity - ra.velocity;
                    float velAlongNormal = glm::dot(relativeVelocity, manifold.normal);

                    // Réveille les entités endormies en cas de collision
                    // Check si la vitesse projetée (ou la vitesse de l'objet qui entre en collision) est assez élevée pour wake up
                    if (ra.isSleeping && !ra.isKinematic) {
                        if(std::abs(velAlongNormal) > PHYSICS_SLEEP_SPEED_THRESHOLD || glm::length(rb.velocity) > PHYSICS_SLEEP_SPEED_THRESHOLD) {
                            ra.isSleeping = false;
                            ra.sleepTimer = 0.0f;
                        }
                    }
                    
                    if (rb.isSleeping && !rb.isKinematic) {
                        if(std::abs(velAlongNormal) > PHYSICS_SLEEP_SPEED_THRESHOLD || glm::length(ra.velocity) > PHYSICS_SLEEP_SPEED_THRESHOLD) {
                            rb.isSleeping = false;
                            rb.sleepTimer = 0.0f;
                        }
                    }

                    // 1. Skip la résolutionentre deux objets endormis
                    if (ra.isSleeping && rb.isSleeping) continue;
                    
                    // 2. Skip les entités trop éloignées
                    float broadphaseMargin = 100.0f;
                    if (!aBox.Intersects(bBox, broadphaseMargin)) continue;

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
            }

            if(!collisionOccurred) break;
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
}