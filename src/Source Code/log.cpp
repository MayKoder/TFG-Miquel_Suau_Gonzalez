#pragma once
#include "Globals.h"

#include "Application.h"
#include "MO_GUI.h"

void log(const char file[], int line, LogType _type, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (EngineExternal != nullptr && EngineExternal->moduleGUI != nullptr)
	{
		EngineExternal->moduleGUI->developerWindow.AddLog(tmp_string, _type);
	}

}