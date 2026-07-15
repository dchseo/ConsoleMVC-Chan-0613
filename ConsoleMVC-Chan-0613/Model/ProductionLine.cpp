#include "ProductionLine.h"
#include <stdexcept>

namespace Model
{
    void ProductionLine::Enqueue(const ProductionJob& job)
    {
        queue_.push_back(job);
    }

    bool ProductionLine::HasCurrentJob() const noexcept
    {
        return !queue_.empty();
    }

    const ProductionJob& ProductionLine::CurrentJob() const
    {
        if (queue_.empty())
        {
            throw std::out_of_range("no current production job");
        }
        return queue_.front();
    }

    ProductionJob ProductionLine::CompleteCurrentJob()
    {
        if (queue_.empty())
        {
            throw std::out_of_range("no current production job");
        }
        ProductionJob job = queue_.front();
        queue_.pop_front();
        return job;
    }

    std::vector<ProductionJob> ProductionLine::GetWaitingJobs() const
    {
        std::vector<ProductionJob> waiting;
        if (queue_.size() > 1)
        {
            waiting.assign(queue_.begin() + 1, queue_.end());
        }
        return waiting;
    }
}
