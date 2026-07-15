#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <limits>

#include "Model/ProductionLine.h"
#include "Model/Repository/InMemoryOrderRepository.h"
#include "Model/Repository/InMemorySampleRepository.h"

#include "Controller/MonitoringController.h"
#include "Controller/OrderController.h"
#include "Controller/SampleController.h"

#include "View/MainMenuView.h"
#include "View/MonitoringView.h"
#include "View/OrderView.h"
#include "View/ProductionLineView.h"
#include "View/SampleView.h"

namespace
{
    int ReadChoice()
    {
        int choice = 0;
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return choice;
    }

    void RunSampleManagement(Controller::SampleController& controller, View::SampleView& view)
    {
        bool back = false;
        while (!back)
        {
            view.ShowMenu();
            switch (ReadChoice())
            {
            case 1:
            {
                const auto input = view.PromptSampleRegistration();
                const bool success = controller.RegisterSample(input.sampleId, input.name, input.avgProductionTime, input.yieldRate);
                view.ShowRegistrationResult(success, input.sampleId);
                break;
            }
            case 2:
                view.ShowSampleList(controller.ListSamples());
                break;
            case 3:
            {
                const auto keyword = view.PromptSearchKeyword();
                view.ShowSampleList(controller.SearchSample(keyword));
                break;
            }
            case 0:
                back = true;
                break;
            default:
                break;
            }
        }
    }

    void RunOrderReservation(Controller::OrderController& controller, View::OrderView& view)
    {
        view.ShowReservationMenu();
        const auto input = view.PromptOrderReservation();
        const auto orderId = controller.ReserveOrder(input.sampleId, input.customerName, input.quantity);
        view.ShowReservationResult(orderId.has_value(), orderId.value_or(""));
    }

    void RunOrderApproval(Controller::OrderController& controller, View::OrderView& view)
    {
        bool back = false;
        while (!back)
        {
            view.ShowReservedOrders(controller.ListReservedOrders());
            view.ShowApprovalMenu();

            switch (ReadChoice())
            {
            case 1:
            {
                const auto orderId = view.PromptOrderId("승인할 주문번호");
                const auto result = controller.ApproveOrder(orderId);
                view.ShowApprovalResult(orderId, result);
                break;
            }
            case 2:
            {
                const auto orderId = view.PromptOrderId("거절할 주문번호");
                const bool success = controller.RejectOrder(orderId);
                view.ShowRejectionResult(success, orderId);
                break;
            }
            case 0:
                back = true;
                break;
            default:
                back = true;
                break;
            }
        }
    }

    void RunMonitoring(Controller::MonitoringController& controller, View::MonitoringView& view)
    {
        bool back = false;
        while (!back)
        {
            view.ShowMenu();
            switch (ReadChoice())
            {
            case 1:
                view.ShowOrderStatusSummary(controller.GetOrderStatusSummary());
                break;
            case 2:
                view.ShowInventoryStatus(controller.GetInventoryStatus());
                break;
            case 0:
                back = true;
                break;
            default:
                back = true;
                break;
            }
        }
    }

    void RunProductionLine(Controller::OrderController& controller, View::ProductionLineView& view)
    {
        bool back = false;
        while (!back)
        {
            view.ShowMenu();
            switch (ReadChoice())
            {
            case 1:
                if (controller.HasCurrentProduction())
                {
                    view.ShowCurrentProduction(controller.GetCurrentProduction());
                }
                else
                {
                    view.ShowNoActiveProductionMessage();
                }
                view.ShowProductionQueue(controller.GetProductionQueue());
                break;
            case 2:
                view.ShowCompletionResult(controller.CompleteCurrentProduction());
                break;
            case 0:
                back = true;
                break;
            default:
                back = true;
                break;
            }
        }
    }

    void RunReleaseProcessing(Controller::OrderController& controller, View::OrderView& view)
    {
        const auto releasable = controller.ListReleasableOrders();
        view.ShowReleasableOrders(releasable);
        if (releasable.empty())
        {
            return;
        }

        const auto orderId = view.PromptOrderId("출고 처리할 주문번호");
        const bool success = controller.ReleaseOrder(orderId);
        view.ShowReleaseResult(success, orderId);
    }
}

int main()
{
    // 소스가 UTF-8(/utf-8 컴파일 옵션)로 컴파일되므로, 콘솔 입출력 코드페이지도
    // UTF-8(65001)로 맞춰야 한글이 깨지지 않는다.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Repository 구현체 선택 지점: 최종 프로젝트에서는 이 두 줄만
    // DataPersistence PoC의 JsonSampleRepository / JsonOrderRepository로 교체하면 된다.
    Model::InMemorySampleRepository sampleRepository;
    Model::InMemoryOrderRepository orderRepository;
    Model::ProductionLine productionLine;

    Controller::SampleController sampleController(sampleRepository);
    Controller::OrderController orderController(orderRepository, sampleRepository, productionLine);
    Controller::MonitoringController monitoringController(orderRepository, sampleRepository);

    View::MainMenuView mainMenuView;
    View::SampleView sampleView;
    View::OrderView orderView;
    View::MonitoringView monitoringView;
    View::ProductionLineView productionLineView;

    bool running = true;
    while (running)
    {
        mainMenuView.ShowMenu();
        switch (mainMenuView.PromptMenuChoice())
        {
        case View::MainMenuOption::SampleManagement:
            RunSampleManagement(sampleController, sampleView);
            break;
        case View::MainMenuOption::PlaceOrder:
            RunOrderReservation(orderController, orderView);
            break;
        case View::MainMenuOption::ApproveOrRejectOrder:
            RunOrderApproval(orderController, orderView);
            break;
        case View::MainMenuOption::Monitoring:
            RunMonitoring(monitoringController, monitoringView);
            break;
        case View::MainMenuOption::ProductionLine:
            RunProductionLine(orderController, productionLineView);
            break;
        case View::MainMenuOption::ReleaseOrder:
            RunReleaseProcessing(orderController, orderView);
            break;
        case View::MainMenuOption::Exit:
            running = false;
            break;
        default:
            mainMenuView.ShowInvalidChoiceMessage();
            break;
        }
    }

    return 0;
}
