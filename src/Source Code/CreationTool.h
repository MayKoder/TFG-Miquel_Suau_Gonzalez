#pragma once


class Tool
{
public:
	Tool(const char* _tName): toolName(_tName) {

	}
	virtual ~Tool() {

	}

	virtual void Use(int button_id) = 0;
	virtual void UseRepeat(int button_id) = 0;

	virtual void DrawEditor() = 0;

	const char* GetName() {
		return toolName;
	}

private:
	const char* toolName;

};

class ToolNode : public Tool
{
public:
	ToolNode(const char* _tName) : Tool(_tName), typeIndex(0)
	{

	}
	virtual ~ToolNode() {

	}

	void Use(int button_id) override;

	void UseRepeat(int button_id) override {
		//EngineExternal->moduleRenderer3D->gridInstance.CreateNode();
	}

	void DrawEditor() override;

private:

	int typeIndex;
	const char* data[3] = { "Single", "Drag", "Line" };
	

};

class  ToolWall : public Tool
{
public:
	ToolWall(const char* _tName) : Tool(_tName)
	{

	}
	virtual ~ToolWall() {

	}

	void Use(int button_id) override;

	void UseRepeat(int button_id) override {};
	void DrawEditor() override {};

private:

};
