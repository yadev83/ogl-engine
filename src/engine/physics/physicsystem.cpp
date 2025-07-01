#include "physicsystem.hpp"

using namespace Engine::Scene;

#include <iostream>

namespace Engine::Physics {
    void PhysicSystem::OnFixedUpdate(float dt) {
        ApplyMotion(dt);
        ResolveCollisions(dt);
    }

    void PhysicSystem::ApplyMotion(float dt) {
        for(auto entityID : GetRegistry().GetEntityIDsWith<Transform, Rigidbody>()) {
            auto& transform = GetRegistry().GetComponent<Transform>(entityID);
            auto& rigidbody = GetRegistry().GetComponent<Rigidbody>(entityID);

            if(rigidbody.isKinematic) continue;
            if(rigidbody.isAffectedByGravity && !rigidbody.onGround) rigidbody.acceleration -= glm::vec3(gravity, 0.0f);

            rigidbody.velocity += (rigidbody.acceleration) * dt;           
            transform.Translate(rigidbody.velocity * unitsPerMeter * dt);

            rigidbody.acceleration = glm::vec3(0.0f);
            rigidbody.velocity *= dampingFactor;
        }
    }

    void PhysicSystem::ResolveCollisions(float dt) {
        auto collidableIDs = GetRegistry().GetEntityIDsWith<Transform, Rigidbody, BoxCollider>();

        for(auto entityID: GetRegistry().GetEntityIDsWith<Rigidbody>()) {
            auto& r = GetRegistry().GetComponent<Rigidbody>(entityID);
            r.onGround = false;
            r.onWall = false;
        }

        for(int iteration = 0; iteration < MAX_PHYSICS_ITERATIONS; ++iteration) {
            bool collisionOccured = false;

            for(size_t i = 0; i < collidableIDs.size(); ++i) {
                EntityID aID = collidableIDs[i];
                auto& ta = GetRegistry().GetComponent<Transform>(aID);
                auto& ca = GetRegistry().GetComponent<BoxCollider>(aID);
                auto& ra = GetRegistry().GetComponent<Rigidbody>(aID);

                // Avant quoi que ce soit, on augmente la duration de tous les collision records de ce collider
                for(auto& [otherID, record] : ca.collisionsList) record.duration += dt;
                for(auto& [otherID, record] : ca.triggersList) record.duration += dt;

                for (size_t j = i + 1; j < collidableIDs.size(); ++j) {
                    EntityID bID = collidableIDs[j];

                    auto& tb = GetRegistry().GetComponent<Transform>(bID);
                    auto& cb = GetRegistry().GetComponent<BoxCollider>(bID);
                    auto& rb = GetRegistry().GetComponent<Rigidbody>(bID);

                    CollisionManifold manifold; // prepare collision manifold to resolve collisions
                    // Si au moins un des deux transforms à subi ne serait-ce qu'une rotation, on passe en check OBB (plus comples, mais prend en compte les rotas.)
                    // if(ta.IsRotated() || tb.IsRotated()) {
                    //     OBB aBox(ta.position, ca.size * ta.scale, ta.rotation), bBox(tb.position, cb.size * tb.scale, tb.rotation);
                    //     manifold = CheckOBBCollision(aBox, bBox);
                    // } else {
                    //     AABB aBox(glm::vec2(ta.position), glm::vec2(ca.size * ta.scale)), bBox(glm::vec2(tb.position), glm::vec2(cb.size * tb.scale));
                    //     manifold = CheckAABBCollision(aBox, bBox);
                    // }

                    // AABB pour l'instant, OBB check est pété, les maths cédur
                    AABB aBox(glm::vec2(ta.position), glm::vec2(ca.size * ta.scale), ta.rotation), bBox(glm::vec2(tb.position), glm::vec2(cb.size * tb.scale), tb.rotation);
                    manifold = CheckAABBCollision(aBox, bBox);

                    // Si pas de collision, on retire les références de chaque collider (si elles existaient) puis on skip la suite
                    if (!manifold.colliding) {
                        if(ca.collisionsList.contains(bID) && ca.collisionsList[bID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            // on sort de l'état de collision, donc on retire l'élément de la liste, et on appelle les callbacks
                            ca.collisionsList.erase(bID);  

                            // Si on a un component "Behaviour" pour l'entité a, on appelle
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnCollisionExit(cb.GetEntity(), manifold);
                            }
                        }                  
                        if(ca.triggersList.contains(bID) && ca.triggersList[bID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            ca.triggersList.erase(bID);

                            // Si on a un component "Behaviour" pour l'entité a, on appelle
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnTriggerExit(cb.GetEntity(), manifold);
                            }
                        }

                        if(cb.collisionsList.contains(aID) && cb.collisionsList[aID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            cb.collisionsList.erase(aID);

                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnCollisionExit(ca.GetEntity(), manifold);
                            }
                        }

                        if(cb.triggersList.contains(aID) && cb.triggersList[aID].duration >= COLLISION_EXPIRE_THRESHOLD) {
                            cb.triggersList.erase(aID);
                            // Si on a un component "Behaviour" pour l'entité b, on appelle
                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnTriggerExit(ca.GetEntity(), manifold);
                            }
                        }

                        continue;
                    }

                    // Traitement de la collision
                    if(ca.isTrigger || cb.isTrigger) {
                        if(!ca.triggersList.contains(bID)) {
                            // Si cb n'est pas dans la liste de triggers de ca, on l'y ajoute
                            ca.triggersList[bID] = {bID, dt};
                            
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnTriggerEnter(cb.GetEntity(), manifold);
                            }
                        } else {
                            // Si cb est déjà dans la liste de triggers, on appelle le script OnStay
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnTriggerStay(cb.GetEntity(), manifold);
                            }
                        }

                        if(!cb.triggersList.contains(aID)) {
                            // Si ca n'est pas dans la liste de triggers de cb, on l'y ajoute
                            cb.triggersList[aID] = {aID, dt};
                            
                            // De plus, si cb à une méthode "onTriggerEnter" on l'appelle maintenant
                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnTriggerEnter(ca.GetEntity(), manifold);
                            }
                        } else {
                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnTriggerStay(ca.GetEntity(), manifold);
                            }
                        }
                        
                        continue;
                    } else { 
                        // Même chose qu'en cas de trigger mais pour les collision brutes
                        if(!ca.collisionsList.contains(bID)) {
                            ca.collisionsList[bID] = {bID, dt};
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnCollisionEnter(cb.GetEntity(), manifold);
                            }
                        } else {
                            if(GetRegistry().HasComponent<Behaviour>(aID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(aID);
                                script.OnCollisionStay(cb.GetEntity(), manifold);
                            }
                        }

                        if(!cb.collisionsList.contains(aID)) {
                            cb.collisionsList[aID] = {aID, dt};
                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnCollisionEnter(ca.GetEntity(), manifold);
                            }
                        } else {
                            if(GetRegistry().HasComponent<Behaviour>(bID)) {
                                auto& script = GetRegistry().GetComponent<Behaviour>(bID);
                                script.OnCollisionStay(ca.GetEntity(), manifold);
                            }
                        }
                    }

                    if (!ra.isKinematic) {
                        if(std::abs(manifold.penetration) >= 0.5f) {
                            glm::vec3 correction = -manifold.normal * manifold.penetration;
                            if (ra.freezePositionX) correction.x = 0.0f;
                            if (ra.freezePositionY) correction.y = 0.0f;
                            if (ra.freezePositionZ) correction.z = 0.0f;
                            ta.position += correction;
                        }

                        if(glm::dot(manifold.normal, glm::vec3(0.0f, 1.0f, 0.0f)) > 0.5f) ra.onGround = true;
                        if(std::abs(glm::dot(manifold.normal, glm::vec3(1.0f, 0.0f, 0.0f))) > 0.5f) ra.onWall = true;

                        collisionOccured = true;
                    } 

                    if (!rb.isKinematic) {
                        if(std::abs(manifold.penetration) >= 0.5f) {
                            glm::vec3 correction = manifold.normal * manifold.penetration;
                            if (rb.freezePositionX) correction.x = 0.0f;
                            if (rb.freezePositionY) correction.y = 0.0f;
                            if (rb.freezePositionZ) correction.z = 0.0f;
                            tb.position += correction;
                        }

                        if(glm::dot(-manifold.normal, glm::vec3(0.0f, 1.0f, 0.0f)) > 0.5f) rb.onGround = true;
                        if(std::abs(glm::dot(-manifold.normal, glm::vec3(1.0f, 0.0f, 0.0f))) > 0.5f) rb.onWall = true;
                        
                        collisionOccured = true;
                    }

                    if((ra.isBounceable || rb.isBounceable) && manifold.colliding) {
                        glm::vec3 directionalInvMassA = {
                            (!ra.isKinematic && !ra.freezePositionX && ra.mass > 0) ? 1.0f / ra.mass : 0.0f,
                            (!ra.isKinematic && !ra.freezePositionY && ra.mass > 0) ? 1.0f / ra.mass : 0.0f,
                            (!ra.isKinematic && !ra.freezePositionZ && ra.mass > 0) ? 1.0f / ra.mass : 0.0f,
                        };
                        glm::vec3 directionalInvMassB = {
                            (!rb.isKinematic && !rb.freezePositionX && rb.mass > 0) ? 1.0f / rb.mass : 0.0f,
                            (!rb.isKinematic && !rb.freezePositionY && rb.mass > 0) ? 1.0f / rb.mass : 0.0f,
                            (!rb.isKinematic && !rb.freezePositionZ && rb.mass > 0) ? 1.0f / rb.mass : 0.0f,
                        };

                        glm::vec3 effectiveInvMass = directionalInvMassA + directionalInvMassB;
                        glm::vec3 relativeVelocity = rb.velocity - ra.velocity;

                        // Composante normale
                        float velocityAlongNormal = glm::dot(relativeVelocity, manifold.normal);
                        glm::vec3 normalVelocity = velocityAlongNormal * manifold.normal;
                        // Calcul de l'impulsion selon la normale de la collision
                        float normalImpulseDenom = glm::dot(effectiveInvMass, manifold.normal * manifold.normal);
                        float restitutionCoeff =  0.5f * (ra.restitution + rb.restitution);
                        float normalImpulseMagnitude = -(1 + restitutionCoeff) * velocityAlongNormal;
                        glm::vec3 normalImpulse = (normalImpulseMagnitude / normalImpulseDenom) * manifold.normal;

                        // Composante tangentielle
                        glm::vec3 tangentVelocity = relativeVelocity - normalVelocity;
                        glm::vec3 tangentImpulse(0.0f);
                        if(glm::length2(tangentVelocity) > 1e-6f) {
                            glm::vec3 tangent = glm::normalize(tangentVelocity);

                            float frictionCoeff = std::sqrt(rb.friction * ra.friction);
                            float velocityAlongTangent = glm::dot(relativeVelocity, tangent);

                            float tangentImpulseMagnitude = -frictionCoeff * velocityAlongTangent;
                            float tangentImpulseDenom = glm::dot(effectiveInvMass, tangent * tangent);

                            if(tangentImpulseDenom > 0.0f) {
                                tangentImpulse = (tangentImpulseMagnitude / tangentImpulseDenom) * tangent;
                            }
                        }

                        glm::vec3 bounceImpulse = normalImpulse + tangentImpulse;

                        if(!ra.isKinematic && ra.mass > 0) {
                            ra.velocity -= bounceImpulse * directionalInvMassA;

                            if(ra.freezePositionX) ra.velocity.x = 0.0f;
                            if(ra.freezePositionY) ra.velocity.y = 0.0f;
                            if(ra.freezePositionZ) ra.velocity.z = 0.0f;
                        }

                        if(!rb.isKinematic && rb.mass > 0) {
                            rb.velocity += bounceImpulse * directionalInvMassB;

                            if(rb.freezePositionX) rb.velocity.x = 0.0f;
                            if(rb.freezePositionY) rb.velocity.y = 0.0f;
                            if(rb.freezePositionZ) rb.velocity.z = 0.0f;
                        }

                        collisionOccured = true;
                    }                   
                }
            }

            if(!collisionOccured) break;
        }
    }

    CollisionManifold PhysicSystem::CheckAABBCollision(const AABB& a, const AABB& b) {
        CollisionManifold manifold;

        glm::vec2 delta = a.center - b.center;
        glm::vec2 overlap = (a.halfSize + b.halfSize) - glm::abs(delta);
        
        if(overlap.x > 0 && overlap.y > 0) {
            manifold.colliding = true;

            if(overlap.x < overlap.y) {
                manifold.penetration = delta.x > 0 ? -overlap.x : overlap.x;
                manifold.normal = glm::vec3(1.0f, 0.0f, 0.0f);
            } else {
                manifold.penetration = delta.y > 0 ? -overlap.y : overlap.y;
                manifold.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        } else {
            manifold.colliding = false;
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