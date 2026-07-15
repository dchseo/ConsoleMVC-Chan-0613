#include "ProductionLineView.h"
#include <iomanip>
#include <iostream>

namespace View
{
    void ProductionLineView::ShowMenu() const
    {
        std::cout << "\n--- [5] 생산 라인 조회 (FIFO) ---\n";
        std::cout << "[1] 현재 생산 및 대기열 조회   [2] 현재 생산 완료 처리(시뮬레이션)   [0] 뒤로\n";
        std::cout << "선택 > ";
    }

    void ProductionLineView::ShowCurrentProduction(const Model::ProductionJob& job) const
    {
        std::cout << "\n현재 처리 중\n";
        std::cout << "주문번호 " << job.orderId << "  시료 " << job.sampleId << '\n';
        std::cout << "부족분 " << job.shortageQuantity << " ea -> 실생산량 " << job.actualQuantity
            << " ea (총 생산시간 " << job.totalProductionTime << " min)\n";
    }

    void ProductionLineView::ShowNoActiveProductionMessage() const
    {
        std::cout << "\n현재 생산 중인 항목이 없습니다.\n";
    }

    void ProductionLineView::ShowProductionQueue(const std::vector<Model::ProductionJob>& queue) const
    {
        std::cout << "\n대기 중인 주문 (FIFO 순)\n";
        if (queue.empty())
        {
            std::cout << "대기 중인 생산 작업이 없습니다.\n";
            return;
        }
        std::cout << std::left
            << std::setw(6) << "순서"
            << std::setw(12) << "주문번호"
            << std::setw(16) << "시료ID"
            << std::setw(10) << "부족분"
            << std::setw(10) << "실생산량" << '\n';

        int order = 1;
        for (const auto& job : queue)
        {
            std::cout << std::left
                << std::setw(6) << order++
                << std::setw(12) << job.orderId
                << std::setw(16) << job.sampleId
                << std::setw(10) << job.shortageQuantity
                << std::setw(10) << job.actualQuantity << '\n';
        }
    }

    void ProductionLineView::ShowCompletionResult(bool success) const
    {
        if (success)
        {
            std::cout << "현재 생산 작업을 완료 처리했습니다. (PRODUCING -> CONFIRMED)\n";
        }
        else
        {
            std::cout << "완료 처리할 생산 작업이 없습니다.\n";
        }
    }
}
