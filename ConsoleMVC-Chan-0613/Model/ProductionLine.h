#pragma once
#include <deque>
#include <string>
#include <vector>

namespace Model
{
    // 생산 라인 큐에 등록되는 단일 생산 작업 (부족분에 대한 생산)
    struct ProductionJob
    {
        std::string orderId;
        std::string sampleId;
        int shortageQuantity = 0;      // 부족분 = 주문 수량 - 재고
        int actualQuantity = 0;        // 실 생산량 = ceil(부족분 / 수율)
        double totalProductionTime = 0.0; // 평균 생산시간 * 실 생산량
    };

    // 단일 생산 라인, FIFO 큐 (선입선출)로 부족분을 순차 생산한다.
    class ProductionLine
    {
    public:
        void Enqueue(const ProductionJob& job);
        bool HasCurrentJob() const noexcept;
        const ProductionJob& CurrentJob() const;
        ProductionJob CompleteCurrentJob();
        std::vector<ProductionJob> GetWaitingJobs() const;

    private:
        std::deque<ProductionJob> queue_;
    };
}
