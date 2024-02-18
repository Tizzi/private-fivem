#pragma once
//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "imgui_internal.h"
#include <Includes.h>
#include <DirectXMath.h>
//#include <imgui.h>
class Renderer
{
public:
	ImFont *TextFont;
public: 
	bool isonscreen(ImVec2 coords); 
	void DrawArrowHead(float x, float y, float angle, float size, ImU32 color);
	void DrawLine(ImVec2 src, ImVec2 dst, ImColor col, float thickness = 1);
	void DrawLineV(ImVec2 src, ImVec2 dst, ImColor col, float thickness = 1);
	void DrawBorder(float x, float y, float w, float h, float thickness, ImColor BorderColor);
	void DrawBox(ImVec2 src, ImVec2 dst, ImColor col, float thickness, float rounding = 0);
	void FillRect(float x, float y, float w, float h, ImColor color, int rounding = 0);
	void WindowFillRect(float x, float y, float w, float h, ImColor color, int rounding = 0);
	void DrawCircle(float x, float y, float radius, ImColor color);
	void DrawCornerBox(float x, float y, float w, float h, int thickness, float coverage, ImColor col);
	bool Draw3DBoxVehicle(Vector3 Position, Vector3 Rotation, Vector3 min, Vector3 max, ImColor BoxColor, bool CrosshairInOnly); 
	void RenderHealthbar(float x, float y, float w, float h, float health, ImColor col, int rounding);
	void Rect(float x, float y, float w, float h, ImColor color, int rounding = 0);
	bool Draw3DBox(Vector3 Position, Vector3 HeadPos, Vector3 FeetPos, Vector3 Rotation, ImColor BoxColor, float HeadHight, float FoodHight, bool CrosshairIn); 
	auto DrawText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center, int style = 3) -> VOID; 
	bool WindowDrawHealthBar(float x, float y, float w, float h, float health, float max, bool Side, ImColor col, bool border);
};

inline Renderer* g_Renderer = new Renderer; 