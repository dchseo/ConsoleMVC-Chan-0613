#pragma once
#include <vector>
#include "../Model/Dtos.h"
#include "../Model/Repository/IOrderRepository.h"
#include "../Model/Repository/ISampleRepository.h"

namespace Controller
{
    class MonitoringController
    {
    public:
        MonitoringController(Model::IOrderRepository& orderRepository, Model::ISampleRepository& sampleRepository);

        // 상태별(RESERVED/PRODUCING/CONFIRMED/RELEASED) 주문 수. REJECTED는 집계 제외.
        Model::OrderStatusSummary GetOrderStatusSummary() const;

        // 시료별 현재 재고 및 여유/부족/고갈 상태.
        std::vector<Model::InventoryStatusItem> GetInventoryStatus() const;

    private:
        Model::IOrderRepository& orderRepository_;
        Model::ISampleRepository& sampleRepository_;
    };
}
