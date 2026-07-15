#pragma once
#include <optional>
#include <string>
#include <vector>
#include "../Model/Dtos.h"
#include "../Model/Order.h"
#include "../Model/ProductionLine.h"
#include "../Model/Repository/IOrderRepository.h"
#include "../Model/Repository/ISampleRepository.h"

namespace Controller
{
    // 시료 주문의 예약/승인/거절/출고와, 승인시 파생되는 생산 라인 등록을 함께 담당한다.
    class OrderController
    {
    public:
        OrderController(Model::IOrderRepository& orderRepository,
            Model::ISampleRepository& sampleRepository,
            Model::ProductionLine& productionLine);

        // 2. 시료 주문
        std::optional<std::string> ReserveOrder(const std::string& sampleId, const std::string& customerName, int quantity);

        // 3. 주문 승인/거절
        std::vector<Model::Order> ListReservedOrders() const;
        Model::OrderApprovalResult ApproveOrder(const std::string& orderId);
        bool RejectOrder(const std::string& orderId);

        // 6. 출고 처리
        std::vector<Model::Order> ListReleasableOrders() const;
        bool ReleaseOrder(const std::string& orderId);

        // 5. 생산 라인 조회
        bool HasCurrentProduction() const;
        const Model::ProductionJob& GetCurrentProduction() const;
        std::vector<Model::ProductionJob> GetProductionQueue() const;
        bool CompleteCurrentProduction();

    private:
        std::string GenerateOrderId();

        Model::IOrderRepository& orderRepository_;
        Model::ISampleRepository& sampleRepository_;
        Model::ProductionLine& productionLine_;
        int nextOrderSequence_ = 1;
    };
}
