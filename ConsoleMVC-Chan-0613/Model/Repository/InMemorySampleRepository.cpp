#include "InMemorySampleRepository.h"
#include <algorithm>
#include <cctype>

namespace Model
{
    namespace
    {
        std::string ToLower(const std::string& text)
        {
            std::string result = text;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return result;
        }
    }

    void InMemorySampleRepository::Add(const Sample& sample)
    {
        samplesById_[sample.GetSampleId()] = sample;
    }

    bool InMemorySampleRepository::Update(const Sample& sample)
    {
        auto it = samplesById_.find(sample.GetSampleId());
        if (it == samplesById_.end())
        {
            return false;
        }
        it->second = sample;
        return true;
    }

    std::optional<Sample> InMemorySampleRepository::FindById(const std::string& sampleId) const
    {
        auto it = samplesById_.find(sampleId);
        if (it == samplesById_.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Sample> InMemorySampleRepository::FindByNameContains(const std::string& keyword) const
    {
        std::vector<Sample> result;
        const std::string loweredKeyword = ToLower(keyword);
        for (const auto& [id, sample] : samplesById_)
        {
            if (ToLower(sample.GetName()).find(loweredKeyword) != std::string::npos)
            {
                result.push_back(sample);
            }
        }
        return result;
    }

    std::vector<Sample> InMemorySampleRepository::GetAll() const
    {
        std::vector<Sample> result;
        result.reserve(samplesById_.size());
        for (const auto& [id, sample] : samplesById_)
        {
            result.push_back(sample);
        }
        return result;
    }
}
