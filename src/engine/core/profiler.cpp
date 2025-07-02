#include "profiler.hpp"

#include "logger.hpp"

namespace Engine::Core {
    void Profiler::Start(const std::string& sectionName) {
        currentSection = sectionName;
        startTime = Clock::now();
    }

    void Profiler::End() {
        auto endTime = Clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        data[currentSection].totalTime += duration;
        data[currentSection].count++;
    }

    void Profiler::Report() const {
        LOG_DEBUG("Profiling report:");
        for (const auto& [section, d] : data) {
            LOG_DEBUG(std::string(" - ") + section + std::string(": ") + std::to_string(d.totalTime) + std::string(" ms total, ") + std::to_string(d.count) + std::string(" calls, avg ") + std::to_string(d.totalTime / d.count) + std::string(" ms"));
        }
    }

    void Profiler::Reset() {
        data.clear();
    }
}