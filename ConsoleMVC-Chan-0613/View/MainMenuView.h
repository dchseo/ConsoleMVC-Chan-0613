#pragma once

namespace View
{
    enum class MainMenuOption
    {
        SampleManagement = 1,
        PlaceOrder = 2,
        ApproveOrRejectOrder = 3,
        Monitoring = 4,
        ProductionLine = 5,
        ReleaseOrder = 6,
        Exit = 0,
        Invalid = -1
    };

    class MainMenuView
    {
    public:
        void ShowMenu() const;
        MainMenuOption PromptMenuChoice() const;
        void ShowInvalidChoiceMessage() const;
    };
}
