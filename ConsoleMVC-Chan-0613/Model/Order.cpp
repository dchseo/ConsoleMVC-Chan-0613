#include "Order.h"

namespace Model
{
    Order::Order(std::string orderId, std::string sampleId, std::string customerName, int quantity)
        : orderId_(std::move(orderId))
        , sampleId_(std::move(sampleId))
        , customerName_(std::move(customerName))
        , quantity_(quantity)
        , status_(OrderStatus::Reserved)
    {
    }

    const std::string& Order::GetOrderId() const noexcept { return orderId_; }
    const std::string& Order::GetSampleId() const noexcept { return sampleId_; }
    const std::string& Order::GetCustomerName() const noexcept { return customerName_; }
    int Order::GetQuantity() const noexcept { return quantity_; }
    OrderStatus Order::GetStatus() const noexcept { return status_; }

    void Order::SetStatus(OrderStatus status) noexcept { status_ = status; }

    std::string ToString(OrderStatus status)
    {
        switch (status)
        {
        case OrderStatus::Reserved: return "RESERVED";
        case OrderStatus::Producing: return "PRODUCING";
        case OrderStatus::Confirmed: return "CONFIRMED";
        case OrderStatus::Released: return "RELEASED";
        case OrderStatus::Rejected: return "REJECTED";
        }
        return "UNKNOWN";
    }
}
