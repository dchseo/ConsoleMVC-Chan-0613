#include "Sample.h"
#include <stdexcept>

namespace Model
{
    Sample::Sample(std::string sampleId, std::string name, double avgProductionTime, double yieldRate, int initialStock)
        : sampleId_(std::move(sampleId))
        , name_(std::move(name))
        , avgProductionTime_(avgProductionTime)
        , yield_(yieldRate)
        , stock_(initialStock)
    {
    }

    const std::string& Sample::GetSampleId() const noexcept { return sampleId_; }
    const std::string& Sample::GetName() const noexcept { return name_; }
    double Sample::GetAvgProductionTime() const noexcept { return avgProductionTime_; }
    double Sample::GetYield() const noexcept { return yield_; }
    int Sample::GetStock() const noexcept { return stock_; }

    void Sample::IncreaseStock(int amount)
    {
        if (amount < 0)
        {
            throw std::invalid_argument("increase amount must be non-negative");
        }
        stock_ += amount;
    }

    bool Sample::DecreaseStock(int amount)
    {
        if (amount < 0)
        {
            throw std::invalid_argument("decrease amount must be non-negative");
        }
        if (amount > stock_)
        {
            return false;
        }
        stock_ -= amount;
        return true;
    }
}
