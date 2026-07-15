#include "MonitoringController.h"

namespace Controller
{
    namespace
    {
        // PoC 단순화: 실제 시스템은 "주문 대비 재고"로 판정하지만, 이 PoC에서는 고정 임계값으로 대체한다.
        constexpr int kLowStockThreshold = 50;
    }

    MonitoringController::MonitoringController(Model::IOrderRepository& orderRepository, Model::ISampleRepository& sampleRepository)
        : orderRepository_(orderRepository)
        , sampleRepository_(sampleRepository)
    {
    }

    Model::OrderStatusSummary MonitoringController::GetOrderStatusSummary() const
    {
        Model::OrderStatusSummary summary{
            { Model::OrderStatus::Reserved, 0 },
            { Model::OrderStatus::Producing, 0 },
            { Model::OrderStatus::Confirmed, 0 },
            { Model::OrderStatus::Released, 0 },
        };

        for (const auto& order : orderRepository_.GetAll())
        {
            if (order.GetStatus() == Model::OrderStatus::Rejected)
            {
                continue;
            }
            ++summary[order.GetStatus()];
        }
        return summary;
    }

    std::vector<Model::InventoryStatusItem> MonitoringController::GetInventoryStatus() const
    {
        std::vector<Model::InventoryStatusItem> items;
        for (const auto& sample : sampleRepository_.GetAll())
        {
            Model::InventoryLevel level = Model::InventoryLevel::Sufficient;
            if (sample.GetStock() <= 0)
            {
                level = Model::InventoryLevel::Depleted;
            }
            else if (sample.GetStock() < kLowStockThreshold)
            {
                level = Model::InventoryLevel::Low;
            }
            items.push_back({ sample, level });
        }
        return items;
    }
}
