#include "SampleController.h"

namespace Controller
{
    SampleController::SampleController(Model::ISampleRepository& sampleRepository)
        : sampleRepository_(sampleRepository)
    {
    }

    bool SampleController::RegisterSample(const std::string& sampleId, const std::string& name, double avgProductionTime, double yieldRate)
    {
        if (sampleRepository_.FindById(sampleId).has_value())
        {
            return false;
        }
        sampleRepository_.Add(Model::Sample(sampleId, name, avgProductionTime, yieldRate, 0));
        return true;
    }

    std::vector<Model::Sample> SampleController::ListSamples() const
    {
        return sampleRepository_.GetAll();
    }

    std::vector<Model::Sample> SampleController::SearchSample(const std::string& keyword) const
    {
        return sampleRepository_.FindByNameContains(keyword);
    }
}
