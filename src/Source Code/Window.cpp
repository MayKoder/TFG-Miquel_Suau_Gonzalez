#include "Window.h"
#include "MMGui.h"
#include "IM_ShaderImporter.h"
#include "IM_FileSystem.h"
#include "MO_ResourceManager.h"

Window::Window() : active(false), name("Dev window"), scrollToBottom(false), txtName("")
{
	txtEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	txtEditor.SetText("");
}

Window::~Window()
{
	logs.clear();
	txtEditor.Delete();
}

void Window::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse))
	{
		DrawConsole();
		DrawShaderEditor();
	}
	ImGui::End();
}

void Window::AddLog(const char* s_msg, LogType _type)
{
	LogMessage message(std::string(s_msg), _type);

	if (logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	logs.push_back(message);
	scrollToBottom = true;
}

void Window::DrawConsole()
{
	ImGui::BeginChild("##consoleArea", ImVec2(0, 350), true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

	float offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
	ImGui::SetCursorPosX(offset);
	if (ImGui::Button("Clear"/*, ImGuiDir_Right*/))
	{
		for (size_t i = 0; i < logs.size(); i++)
		{
			logs[i].msg.clear();
		}
		logs.clear();
	}
	ImGui::SameLine();

	offset -= (ImGui::CalcTextSize("Clear").x / 2) - 7;
	ImGui::SetCursorPosX(offset);

	ImGui::GreySeparator();

	if (ImGui::BeginChild("##consolePrintArea"))
	{
		LogMessage* cLog = nullptr;

		for (unsigned int i = 0; i < logs.size(); ++i) //Rendering all logs is dumb, need to find a way to ignore out-of-view logs
		{
			cLog = &logs[i];
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
			ImGui::TextWrapped(cLog->msg.c_str());

			if (logs[i].prints > 1)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(cLog->prints).c_str()).x);
				ImGui::Text("%i", cLog->prints);
			}
		}

		if (scrollToBottom && !ImGui::IsWindowFocused())
		{
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}
	}
	ImGui::EndChild();

	ImGui::EndChild();
}

LogMessage::LogMessage(std::string& s_msg, LogType _type) : prints(1), lType(_type)
{
	msg = s_msg;
}

bool LogMessage::EqualsStr(const char* cmp)
{
	return (strcmp(msg.c_str(), cmp) == 0) ? true : false;
}

#pragma region ShaderPart
void Window::DrawShaderEditor()
{
	ImGui::BeginChild("Shader editor", ImVec2(0, 350), true);

	ImGui::Dummy(ImVec2(10, 10));
	if (ImGui::Button("Save and compile shader"))
	{
		//Check for errors
		TempShader vertexShaderPair, fragmentShaderPair;

		TextEditor::ErrorMarkers markers;
		txtEditor.SetErrorMarkers(markers);

		if (ShaderImporter::CheckForErrors(txtEditor.GetText(), vertexShaderPair, fragmentShaderPair) == true)
		{
			//Save glsl
			FileSystem::Save(txtName.c_str(), &txtEditor.GetText()[0], txtEditor.GetText().length(), false);

			//Find resource
			uint uid = EngineExternal->moduleResources->GetMetaUID(EngineExternal->moduleResources->GetMetaPath(txtName.c_str()).c_str());
			Resource* res = EngineExternal->moduleResources->GetResourceFromUID(uid);

			bool new_resource = false;

			//Clear resource
			if (res != nullptr)
				res->UnloadFromMemory();
			else
			{
				new_resource = true;
				res = EngineExternal->moduleResources->RequestResource(uid, Resource::Type::SHADER);
			}

			//Save .shdr and reimport data
			ShaderImporter::Import(&txtEditor.GetText()[0], txtEditor.GetText().length(), dynamic_cast<ResourceShader*>(res), txtName.c_str());

			if (new_resource)
				EngineExternal->moduleResources->UnloadResource(uid);
		}
	}

	if (!txtName.empty())
		txtEditor.Render(txtName.c_str());

	ImGui::EndChild();
}

void Window::SetErrorsOnScreen(const char* infoLog)
{
	TextEditor::ErrorMarkers markers;

	std::vector<std::pair<int, std::string>> errors;
	SplitErrors(infoLog, errors);

	for (size_t i = 0; i < errors.size(); i++)
	{
		markers.insert(errors[i]);
	}

	txtEditor.SetErrorMarkers(markers);
}

void Window::SplitErrors(const char* infoLog, std::vector<std::pair<int, std::string>>& error_list)
{
	std::string tmp = infoLog;

	while (tmp.find("0(") != std::string::npos)
	{
		size_t error_line_pos = tmp.find("0(");
		std::string error_line = tmp.substr(error_line_pos + 2, 2);

		std::pair<int, std::string> error;
		error.first = std::stoi(error_line);

		size_t found = tmp.find("\n");
		if (found != std::string::npos)
		{
			std::string error_text = tmp.substr(7, found - 7);

			error.second = error_text;

			tmp.erase(tmp.begin(), tmp.begin() + found + 1);
		}
		else
		{
			error.second = tmp.substr(0);
			tmp.erase(tmp.begin(), tmp.begin() + found + 1);
		}

		error_list.push_back(error);
	}
}

void Window::SetTextFromFile(const char* path)
{
	txtEditor.Delete();
	char* buffer = nullptr;
	FileSystem::LoadToBuffer(path, &buffer);

	const TextEditor::LanguageDefinition* lng;
	lng = TextEditor::LanguageDefinition::GLSL();
	txtEditor.SetLanguageDefinition(lng);
	//std::string test = FileSystem::FileToText(path); //Can't use physFS because it's

	if (buffer != nullptr)
	{
		txtName = path;
		txtEditor.SetText(buffer);

		RELEASE_ARRAY(buffer);
	}
	else
	{
		txtName = "";
	}
}
#pragma endregion
