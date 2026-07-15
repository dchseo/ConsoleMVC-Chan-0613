#include "InMemoryOrderRepository.h"

namespace Model
{
    void InMemoryOrderRepository::Add(const Order& order)
    {
        ordersById_[order.GetOrderId()] = order;
    }

    bool InMemoryOrderRepository::Update(const Order& order)
    {
        auto it = ordersById_.find(order.GetOrderId());
        if (it == ordersById_.end())
        {
            return false;
        }
        it->second = order;
        return true;
    }

    std::optional<Order> InMemoryOrderRepository::FindById(const std::string& orderId) const
    {
        auto it = ordersById_.find(orderId);
        if (it == ordersById_.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Order> InMemoryOrderRepository::FindByStatus(OrderStatus status) const
    {
        std::vector<Order> result;
        for (const auto& [id, order] : ordersById_)
        {
            if (order.GetStatus() == status)
            {
                result.push_back(order);
            }
        }
        return result;
    }

    std::vector<Order> InMemoryOrderRepository::GetAll() const
    {
        std::vector<Order> result;
        result.reserve(ordersById_.size());
        for (const auto& [id, order] : ordersById_)
        {
            result.push_back(order);
        }
        return result;
    }
}
