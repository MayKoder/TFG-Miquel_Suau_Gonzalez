#include"MMGui.h"

void ImGui::CenterNextElement(ImVec2 windowSize, float widthRatio)
{
    float bSize = windowSize.x * widthRatio;
    ImGui::SetCursorPosX((windowSize.x / 2) - (bSize / 2));
    ImGui::SetNextItemWidth(bSize);
}

bool ImGui::MiddleButton(const char* title, ImVec2 windowSize, float widthRatio)
{
    float bSize = windowSize.x * widthRatio;
    ImGui::SetCursorPosX((windowSize.x / 2) - (bSize / 2));
    return ImGui::Button(title, ImVec2(bSize, ImGui::CalcTextSize(title).y * 2.f));
}

void ImGui::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
        *v = !*v;
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
    else
        col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

void ImGui::GreySeparator()
{
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Separator, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
    ImGui::Separator();
    ImGui::PopStyleColor();
}

void ImGui::AddUnderLine(ImColor col_)
{
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    min.y = max.y;
    ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
}

void ImGui::TextURL(const char* name_, const char* URL_, uint8_t sameLineBefore, uint8_t sameLineAfter)
{
    if (1 == sameLineBefore) 
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); 

    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
    ImGui::Text(name_);
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered())
    {
        if (ImGui::IsMouseClicked(0))
        {
            ShellExecute(0, 0, URL_, 0, 0, SW_SHOW);
        }
        AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
    }
    else
    {
        AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
    }
    if (1 == sameLineAfter) 
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
}

//ImGuiID ImGui::DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspace_flags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* window_class)
//{
//    if (viewport == NULL)
//        viewport = GetMainViewport();
//
//    SetNextWindowPos(position);
//    SetNextWindowSize(size);
//    SetNextWindowViewport(viewport->ID);
//
//    ImGuiWindowFlags host_window_flags = 0;
//    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
//    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//        host_window_flags |= ImGuiWindowFlags_NoBackground;
//
//    char label[32];
//    ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);
//
//    PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//    Begin(label, NULL, host_window_flags);
//    PopStyleVar(3);
//
//    ImGuiID dockspace_id = GetID("DockSpace");
//    DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);
//    End();
//
//    return dockspace_id;
//}

ImVec2 ImGui::GetButtonSize(const char* text, const ImVec2 size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return ImVec2(0.f, 0.f);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(text);
    const ImVec2 label_size = CalcTextSize(text, NULL, true);

    return CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
}
