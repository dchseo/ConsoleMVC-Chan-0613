#pragma once
#include <string>

namespace Model
{
    class Sample
    {
    public:
        Sample() = default;
        Sample(std::string sampleId, std::string name, double avgProductionTime, double yieldRate, int initialStock = 0);

        const std::string& GetSampleId() const noexcept;
        const std::string& GetName() const noexcept;
        double GetAvgProductionTime() const noexcept;
        double GetYield() const noexcept;
        int GetStock() const noexcept;

        void IncreaseStock(int amount);
        bool DecreaseStock(int amount);

    private:
        std::string sampleId_;
        std::string name_;
        double avgProductionTime_ = 0.0;
        double yield_ = 1.0;
        int stock_ = 0;
    };
}
