#pragma once
#include <string>
#include <vector>
#include "../Model/Repository/ISampleRepository.h"

namespace Controller
{
    class SampleController
    {
    public:
        explicit SampleController(Model::ISampleRepository& sampleRepository);

        bool RegisterSample(const std::string& sampleId, const std::string& name, double avgProductionTime, double yieldRate);
        std::vector<Model::Sample> ListSamples() const;
        std::vector<Model::Sample> SearchSample(const std::string& keyword) const;

    private:
        Model::ISampleRepository& sampleRepository_;
    };
}
