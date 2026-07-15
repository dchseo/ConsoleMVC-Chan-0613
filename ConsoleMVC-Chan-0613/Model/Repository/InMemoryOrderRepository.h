#pragma once
#include <unordered_map>
#include "IOrderRepository.h"

namespace Model
{
    // 이번 PoC의 유일한 구현체. 프로세스 종료시 데이터는 유지되지 않는다.
    class InMemoryOrderRepository : public IOrderRepository
    {
    public:
        void Add(const Order& order) override;
        bool Update(const Order& order) override;
        std::optional<Order> FindById(const std::string& orderId) const override;
        std::vector<Order> FindByStatus(OrderStatus status) const override;
        std::vector<Order> GetAll() const override;

    private:
        std::unordered_map<std::string, Order> ordersById_;
    };
}
