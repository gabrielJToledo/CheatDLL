#include "Menu.h"
#include "../ImGui/imgui.h"

ImVec4 DarkViolet{ 148, 0, 211, 1.000 };
ImVec4 DarkBlue{ 0, 0, 138, 1 };
ImVec4 Black{ 0, 0, 0, 102 };
ImVec4 Gray{ 27, 27, 27, 255 };
ImVec4 C_Purple{ 59, 0, 144, 255 };
ImVec4 Red{ 255, 0, 0, 0 };

void Menu::Render()
{
	ImGui::SetNextWindowSize({ 707, 488 });
	ImGui::Begin("Hello World", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	{

	}
	ImGui::End();
}

void Menu::Style()
{

	ImGui::PushStyleColor(ImGuiCol_Text, DarkViolet);
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, Red);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, DarkBlue);
	ImGui::PushStyleColor(ImGuiCol_Button, Black);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Gray);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, Gray);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Black);

}
