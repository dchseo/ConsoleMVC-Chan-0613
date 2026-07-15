#pragma once
#include <unordered_map>
#include "ISampleRepository.h"

namespace Model
{
    // 이번 PoC의 유일한 구현체. 프로세스 종료시 데이터는 유지되지 않는다.
    class InMemorySampleRepository : public ISampleRepository
    {
    public:
        void Add(const Sample& sample) override;
        bool Update(const Sample& sample) override;
        std::optional<Sample> FindById(const std::string& sampleId) const override;
        std::vector<Sample> FindByNameContains(const std::string& keyword) const override;
        std::vector<Sample> GetAll() const override;

    private:
        std::unordered_map<std::string, Sample> samplesById_;
    };
}
