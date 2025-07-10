#include "sprite.hpp"

namespace Engine::Graphics {
    void SpriteAnimator::AddAnimation(const std::string& name, const SpriteAnimation& animation) {
        mAnimations[name] = animation;
    }

    SpriteAnimation& SpriteAnimator::GetAnimation(const std::string& name) {
        auto it = mAnimations.find(name);
        if(it == mAnimations.end()) throw std::runtime_error("animation with name" + name + "not found");

        return it->second;
    }

    void SpriteAnimator::Play(const std::string& name, bool resetIfSame) {
        if(!resetIfSame && name.compare(mCurrentAnimationName) == 0) return;

        auto it = mAnimations.find(name);
        if(it != mAnimations.end()) {
            mCurrentAnimationName = name;
            mCurrentFrameIndex = 0;
            mTimer = 0.0f;
            mPlaying = true;
        }
    }

    void SpriteAnimator::Stop() {
        mPlaying = false;
    }

    std::string SpriteAnimator::GetCurrentAnimationName() const {
        return mCurrentAnimationName;
    }

    bool SpriteAnimator::IsPlaying() const {
        return mPlaying;
    }

    Graphics::Texture* SpriteAnimator::GetCurrentFrame() const {
        if(!mPlaying) return nullptr;
        
        auto it = mAnimations.find(mCurrentAnimationName);
        if(it == mAnimations.end()) return nullptr;

        const auto& animation = it->second;
        if(animation.frames.empty()) return nullptr;

        return animation.frames[mCurrentFrameIndex];
    }

    void SpriteAnimator::Update(float dt) {
        if(!mPlaying) return;

        auto it = mAnimations.find(mCurrentAnimationName);
        if(it == mAnimations.end()) return;

        SpriteAnimation& anim = it->second;
        mTimer += dt;
        if(mTimer >= anim.frameDuration) {
            mTimer = 0.0f;
            mCurrentFrameIndex++;
            if(mCurrentFrameIndex >= anim.frames.size()) {
                if(anim.loop) {
                    mCurrentFrameIndex = 0;
                } else {
                    mCurrentFrameIndex = anim.frames.size() - 1;
                    mPlaying = false;
                }
            }
        }
    }

    void SpriteAnimationSystem::OnUpdate(float deltaTime) {
        for (auto entityID : GetRegistry().GetEntityIDsWith<SpriteAnimator, Sprite>()) {
            auto& animator = GetRegistry().GetComponent<SpriteAnimator>(entityID);
            if(!animator.enabled) continue;

            animator.Update(deltaTime);

            Graphics::Texture* frame = animator.GetCurrentFrame();
            if(frame) {
                auto& sprite = GetRegistry().GetComponent<Sprite>(entityID);
                sprite.material.texture = frame;
            }
        }
    }
}