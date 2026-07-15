#include "OrderController.h"
#include <cmath>
#include <iomanip>
#include <sstream>

namespace Controller
{
    OrderController::OrderController(Model::IOrderRepository& orderRepository,
        Model::ISampleRepository& sampleRepository,
        Model::ProductionLine& productionLine)
        : orderRepository_(orderRepository)
        , sampleRepository_(sampleRepository)
        , productionLine_(productionLine)
    {
    }

    std::string OrderController::GenerateOrderId()
    {
        std::ostringstream oss;
        oss << "ORD-" << std::setw(4) << std::setfill('0') << nextOrderSequence_++;
        return oss.str();
    }

    std::optional<std::string> OrderController::ReserveOrder(const std::string& sampleId, const std::string& customerName, int quantity)
    {
        if (quantity <= 0 || !sampleRepository_.FindById(sampleId).has_value())
        {
            return std::nullopt;
        }

        const std::string orderId = GenerateOrderId();
        orderRepository_.Add(Model::Order(orderId, sampleId, customerName, quantity));
        return orderId;
    }

    std::vector<Model::Order> OrderController::ListReservedOrders() const
    {
        return orderRepository_.FindByStatus(Model::OrderStatus::Reserved);
    }

    Model::OrderApprovalResult OrderController::ApproveOrder(const std::string& orderId)
    {
        Model::OrderApprovalResult result;

        auto orderOpt = orderRepository_.FindById(orderId);
        if (!orderOpt.has_value() || orderOpt->GetStatus() != Model::OrderStatus::Reserved)
        {
            result.message = "승인 가능한 주문(RESERVED)이 아닙니다.";
            return result;
        }
        Model::Order order = *orderOpt;

        auto sampleOpt = sampleRepository_.FindById(order.GetSampleId());
        if (!sampleOpt.has_value())
        {
            result.message = "주문한 시료 정보를 찾을 수 없습니다.";
            return result;
        }
        Model::Sample sample = *sampleOpt;

        if (sample.GetStock() >= order.GetQuantity())
        {
            sample.DecreaseStock(order.GetQuantity());
            sampleRepository_.Update(sample);

            order.SetStatus(Model::OrderStatus::Confirmed);
            orderRepository_.Update(order);

            result.success = true;
            result.resultingStatus = Model::OrderStatus::Confirmed;
            result.message = "재고가 충분하여 즉시 출고 대기(CONFIRMED) 상태로 전환되었습니다.";
            return result;
        }

        const int shortage = order.GetQuantity() - sample.GetStock();
        const int actualQuantity = static_cast<int>(std::ceil(shortage / sample.GetYield()));
        const double totalProductionTime = sample.GetAvgProductionTime() * actualQuantity;

        Model::ProductionJob job;
        job.orderId = order.GetOrderId();
        job.sampleId = sample.GetSampleId();
        job.shortageQuantity = shortage;
        job.actualQuantity = actualQuantity;
        job.totalProductionTime = totalProductionTime;
        productionLine_.Enqueue(job);

        order.SetStatus(Model::OrderStatus::Producing);
        orderRepository_.Update(order);

        result.success = true;
        result.resultingStatus = Model::OrderStatus::Producing;
        result.shortageQuantity = shortage;
        result.actualProductionQuantity = actualQuantity;
        result.totalProductionTime = totalProductionTime;
        result.message = "재고 부족으로 생산 라인(PRODUCING)에 등록되었습니다.";
        return result;
    }

    bool OrderController::RejectOrder(const std::string& orderId)
    {
        auto orderOpt = orderRepository_.FindById(orderId);
        if (!orderOpt.has_value() || orderOpt->GetStatus() != Model::OrderStatus::Reserved)
        {
            return false;
        }
        Model::Order order = *orderOpt;
        order.SetStatus(Model::OrderStatus::Rejected);
        orderRepository_.Update(order);
        return true;
    }

    std::vector<Model::Order> OrderController::ListReleasableOrders() const
    {
        return orderRepository_.FindByStatus(Model::OrderStatus::Confirmed);
    }

    bool OrderController::ReleaseOrder(const std::string& orderId)
    {
        auto orderOpt = orderRepository_.FindById(orderId);
        if (!orderOpt.has_value() || orderOpt->GetStatus() != Model::OrderStatus::Confirmed)
        {
            return false;
        }
        Model::Order order = *orderOpt;
        order.SetStatus(Model::OrderStatus::Released);
        orderRepository_.Update(order);
        return true;
    }

    bool OrderController::HasCurrentProduction() const
    {
        return productionLine_.HasCurrentJob();
    }

    const Model::ProductionJob& OrderController::GetCurrentProduction() const
    {
        return productionLine_.CurrentJob();
    }

    std::vector<Model::ProductionJob> OrderController::GetProductionQueue() const
    {
        return productionLine_.GetWaitingJobs();
    }

    bool OrderController::CompleteCurrentProduction()
    {
        if (!productionLine_.HasCurrentJob())
        {
            return false;
        }
        Model::ProductionJob job = productionLine_.CompleteCurrentJob();

        auto sampleOpt = sampleRepository_.FindById(job.sampleId);
        auto orderOpt = orderRepository_.FindById(job.orderId);
        if (!sampleOpt.has_value() || !orderOpt.has_value())
        {
            return false;
        }

        Model::Sample sample = *sampleOpt;
        Model::Order order = *orderOpt;

        // 실 생산량만큼 재고를 채운 뒤, 해당 주문 수량만큼 다시 소진하여 CONFIRMED로 전환한다.
        sample.IncreaseStock(job.actualQuantity);
        sample.DecreaseStock(order.GetQuantity());
        sampleRepository_.Update(sample);

        order.SetStatus(Model::OrderStatus::Confirmed);
        orderRepository_.Update(order);

        return true;
    }
}
