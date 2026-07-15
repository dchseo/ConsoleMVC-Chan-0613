#pragma once
#include <vector>
#include "../Model/ProductionLine.h"

namespace View
{
    class ProductionLineView
    {
    public:
        void ShowMenu() const;
        void ShowCurrentProduction(const Model::ProductionJob& job) const;
        void ShowNoActiveProductionMessage() const;
        void ShowProductionQueue(const std::vector<Model::ProductionJob>& queue) const;
        void ShowCompletionResult(bool success) const;
    };
}
