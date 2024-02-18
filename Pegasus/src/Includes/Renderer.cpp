#include <Renderer.h>
#include <GameMath.h>

// Function to remove '^' and the following number from a const char* string
void RemoveCorlorFormatText(std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '^') {
			str.erase(i, 2);
			i--;  // Decrement i to account for the erased character
		}
	}
}
inline bool Renderer::isonscreen(ImVec2 coords)
{
	static float ScreenCX = ImGui::GetIO().DisplaySize.x;
	static float ScreenCY = ImGui::GetIO().DisplaySize.y;

	if (coords.x < 0.1f || coords.y < 0.1 || coords.x > ScreenCX || coords.y > ScreenCY)
	{
		return false;
	}
	else {
		return true;
	}
}
inline bool isInScreen(const ImVec2& from, const ImVec2& to) {

	static float ScreenCX = ImGui::GetIO().DisplaySize.x;
	static float ScreenCY = ImGui::GetIO().DisplaySize.y;

	if ((from.x >= 0) && (from.x <= (ScreenCX)) & (from.y >= 0) & (from.y <= (ScreenCY)))
	{
		if ((to.x >= 0) & (to.x <= (ScreenCX)) & (to.y >= 0) & (to.y <= (ScreenCY)))
		{
			return true;
		}
	}
	return false;
}
void Renderer::DrawArrowHead(float x, float y, float angle, float size, ImU32 color)
{

	ImVec2 A = ImVec2(x, y);
	ImVec2 B = ImVec2(x + 15 * std::cos(angle - 3.14 / 6), y + 15 * std::sin(angle - 3.14 / 6));
	ImVec2 C = ImVec2(x + 15 * std::cos(angle + 3.14 / 6), y + 15 * std::sin(angle + 3.14 / 6));
	ImVec2 c0 = { (A.x + B.x + C.x) / 3.f , (A.y + B.y + C.y) / 3.f };

	//ImGui::GetBackgroundDrawList()->AddTriangleFilled(A, B, c0, ImColor{ 255,255,255 });
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(A, B, c0, color* 0.67f);
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(A, c0, C, color);
}
void Renderer::DrawBox(ImVec2 src, ImVec2 dst, ImColor col, float thickness, float rounding)
{
	if (this->isonscreen(dst) && this->isonscreen(src))
	{
		ImGui::GetBackgroundDrawList()->AddRect({ src.x, src.y }, { dst.x, dst.y }, col);
		ImGui::GetBackgroundDrawList()->AddRect({ src.x + 1, src.y + 1 }, { dst.x + 1, dst.y + 1 }, ImColor{0,0,0},0.5f);
	}
}
void Renderer::DrawCornerBox(float x, float y, float w, float h, int thickness, float coverage, ImColor col)
{
	DrawLine({ x, y }, { x + (w * coverage), y }, col, thickness);
	DrawLine({ x, y }, { x, y + (h * coverage / 2) }, col, thickness);
	//DrawLine({ x, y - thickness }, { x + (w * coverage), y - thickness }, ImColor(0,0,0), thickness);
	//DrawLine({ x - thickness, y }, { x- thickness, y + (h * coverage / 2) }, ImColor(0, 0, 0), thickness);

	DrawLine({ x, y + h }, { x + (w * coverage), y + h }, col, thickness);
	DrawLine({ x, y + h }, { x, (y + h) - (h * coverage / 2) }, col, thickness);
	//DrawLine({ x, y + h + thickness }, { x + (w * coverage), y + h + thickness }, ImColor(0, 0, 0), thickness);
	//DrawLine({ x - thickness, y + h }, { x - thickness, (y + h) - (h * coverage / 2) }, ImColor(0, 0, 0), thickness);

	DrawLine({ x + w, y }, { (x + w) - (w * coverage), y }, col, thickness);
	DrawLine({ x + w, y }, { x + w, y + (h * coverage / 2) }, col, thickness);
	//DrawLine({ x + w, y - thickness}, { (x + w) - (w * coverage), y - thickness}, ImColor(0, 0, 0), thickness);
	//DrawLine({ x + w + thickness , y }, { x + w + thickness, y + (h * coverage / 2) }, ImColor(0, 0, 0), thickness);

	DrawLine({ x + w, y + h }, { (x + w) - (w * coverage), y + h }, col, thickness);
	DrawLine({ x + w, y + h }, { x + w, (y + h) - (h * coverage / 2) }, col, thickness);
	//DrawLine({ x + w, y + h + thickness}, { (x + w) - (w * coverage), y + h + thickness }, ImColor(0, 0, 0), thickness);
	//DrawLine({ x + w + thickness, y + h }, { x + w + thickness, (y + h) - (h * coverage / 2) }, ImColor(0, 0, 0), thickness);
}

bool Renderer::Draw3DBoxVehicle(Vector3 Position, Vector3 Rotation, Vector3 min, Vector3 max, ImColor BoxColor, bool CrosshairInOnly) {
	float rot = acosf(Rotation.x) * 180.0f / DirectX::XM_PI;
	if (asinf(Rotation.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	ImVec2 t[8];
	Vector4 Cosines(0, 0, 0, 0);
	Vector4 Sines(0, 0, 0, 0);
	Vector4 Corners = Vector4(DirectX::XMConvertToRadians(rot - 35), DirectX::XMConvertToRadians(rot - 135.0f), DirectX::XMConvertToRadians(rot + 35), DirectX::XMConvertToRadians(rot + 135.0f));
	Cosines.x = cosf(Corners.x);
	Cosines.y = cosf(Corners.y);
	Cosines.z = cosf(Corners.z);
	Cosines.w = cosf(Corners.w);
	Sines.x = sinf(Corners.x);
	Sines.y = sinf(Corners.y);
	Sines.z = sinf(Corners.z);
	Sines.w = sinf(Corners.w);

	float radius = 0.5;

	Vector3 HeadLeftForward = Vector3(min.x + Cosines.x * radius, min.y + Sines.x * radius, min.z + 0.0f);
	Vector3 HeadLeftBackward = Vector3(min.x + Cosines.y * radius, min.y + Sines.y * radius, min.z + 0.0f);
	Vector3 HeadRightForward = Vector3(min.x + Cosines.z * radius, min.y + Sines.z * radius, min.z + 0.0f);
	Vector3 HeadRightBackward = Vector3(min.x + Cosines.w * radius, min.y + Sines.w * radius, min.z + 0.0f);

	Vector3 FeetLeftForward = Vector3(max.x + Cosines.x * radius, max.y + Sines.x * radius, max.z + 0.0f);
	Vector3 FeetLeftBackward = Vector3(max.x + Cosines.y * radius, max.y + Sines.y * radius, max.z + 0.0f);
	Vector3 FeetRightForward = Vector3(max.x + Cosines.z * radius, max.y + Sines.z * radius, max.z + 0.0f);
	Vector3 FeetRightBackward = Vector3(max.x + Cosines.w * radius, max.y + Sines.w * radius, max.z + 0.0f);

	t[0] = g_Math->WorldToScreen(HeadLeftForward);
	t[1] = g_Math->WorldToScreen(HeadLeftBackward);
	t[2] = g_Math->WorldToScreen(HeadRightForward);
	t[3] = g_Math->WorldToScreen(HeadRightBackward);

	t[4] = g_Math->WorldToScreen(FeetLeftForward);
	t[5] = g_Math->WorldToScreen(FeetLeftBackward);
	t[6] = g_Math->WorldToScreen(FeetRightForward);
	t[7] = g_Math->WorldToScreen(FeetRightBackward);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[1].x, t[1].y)))
		DrawLine({ t[0].x , t[0].y }, { t[1].x , t[1].y }, BoxColor);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[2].x, t[2].y)))
		DrawLine({ t[0].x , t[0].y }, { t[2].x , t[2].y }, BoxColor);

	if (isInScreen(ImVec2(t[1].x, t[1].y), ImVec2(t[3].x, t[3].y)))
		DrawLine({ t[1].x , t[1].y }, { t[3].x , t[3].y }, BoxColor);

	if (isInScreen(ImVec2(t[2].x, t[2].y), ImVec2(t[3].x, t[3].y)))
		DrawLine({ t[2].x , t[2].y }, { t[3].x , t[3].y }, BoxColor);

	if (isInScreen(ImVec2(t[4].x, t[4].y), ImVec2(t[5].x, t[5].y)))
		DrawLine({ t[4].x , t[4].y }, { t[5].x , t[5].y }, BoxColor);

	if (isInScreen(ImVec2(t[4].x, t[4].y), ImVec2(t[6].x, t[6].y)))
		DrawLine({ t[4].x , t[4].y }, { t[6].x , t[6].y }, BoxColor);

	if (isInScreen(ImVec2(t[5].x, t[5].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[5].x , t[5].y }, { t[7].x , t[7].y }, BoxColor);

	if (isInScreen(ImVec2(t[6].x, t[6].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[6].x , t[6].y }, { t[7].x , t[7].y }, BoxColor);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[4].x, t[4].y)))
		DrawLine({ t[0].x , t[0].y }, { t[4].x , t[4].y }, BoxColor);

	if (isInScreen(ImVec2(t[1].x, t[1].y), ImVec2(t[5].x, t[5].y)))
		DrawLine({ t[1].x , t[1].y }, { t[5].x , t[5].y }, BoxColor);

	if (isInScreen(ImVec2(t[2].x, t[2].y), ImVec2(t[6].x, t[6].y)))
		DrawLine({ t[2].x , t[2].y }, { t[6].x , t[6].y }, BoxColor);

	if (isInScreen(ImVec2(t[3].x, t[3].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[3].x , t[3].y }, { t[7].x , t[7].y }, BoxColor);

	return true;

} 
void DrawOutlinedText(ImFont* font, const std::string& text, const ImVec2& pos, float size, ImU32 color, int style)
{
	if (style == 1 || style == 3)//shadow top
	ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(pos.x - 1, pos.y - 1), IM_COL32(0, 0, 0, 128), text.c_str());
	if (style == 2 || style == 3)//shadow bottom
	ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 128), text.c_str());
	//real text
	ImGui::GetBackgroundDrawList()->AddText(font, size, pos, ImGui::GetColorU32(color), text.c_str());

}
// Function to convert custom color codes to ImGui color codes
inline ImU32 GetColorFromCode(char code, ImU32 default_) {
	switch (code) {
	case '1': return IM_COL32(255, 68, 51, 255); //^1 Red orange
	case '2': return IM_COL32(144, 238, 144, 255); //^2 Light Green
	case '3': return IM_COL32(255, 192, 0, 255); // ^3  Light Yellow
	case '4': return IM_COL32(0, 0, 255, 255); //^4  Dark Blue
	case '5': return IM_COL32(0, 150, 255, 255); //^5 Light Blue
	case '6': return IM_COL32(127, 0, 255, 255); // ^6 Violet
	case '7': return IM_COL32(255, 255, 255, 255); // ^7 // White
	case '8': return IM_COL32(136, 8, 8, 255); // ^8 // Blood Red
	case '9': return IM_COL32(255, 0, 255, 255); // ^9 // Fuchsia
	default: return default_; // Default to original setting
	}
}
auto Renderer::DrawText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center, int style) -> VOID
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int index = 0;
	
	while (std::getline(stream, line))
	{
		std::string copiedText(line);
		auto OrgTextFormat = copiedText.c_str();
		RemoveCorlorFormatText(line);
		
		ImVec2 textSize = pFont->CalcTextSizeA(size, 300, 0.0f, line.c_str());
		ImVec2 drawPos;
		if(center) drawPos = ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * index);
		else drawPos = ImVec2(pos.x, pos.y + textSize.y * index);

		ImGuiTextBuffer buffer;
		ImU32 textColor = color;
		
		while (*OrgTextFormat) {
			if (*OrgTextFormat == '^' && *(OrgTextFormat + 1) >= '0' && *(OrgTextFormat + 1) <= '9')
			{
				if (!buffer.empty())
				{
					DrawOutlinedText(pFont, buffer.c_str(), drawPos, size, textColor, style);//ImGui::GetColorU32(color)
					ImVec2 thisSize = pFont->CalcTextSizeA(size, 300, 0.0f, buffer.c_str());
					drawPos.x += thisSize.x;//next draw pos after this text
					buffer.clear();//done draw clear for retrieve next text
				}
				textColor = GetColorFromCode(*(OrgTextFormat + 1), color);//convert this color 
				OrgTextFormat += 2; // Skip the '^' and the color code
			}
			else {
				buffer.appendf("%c", *OrgTextFormat);
				OrgTextFormat++;
			}
		}
		DrawOutlinedText(pFont, buffer.c_str(), drawPos, size, textColor, style);

		y = pos.y + textSize.y * (index + 1);
		index++;
	}
}

bool Renderer::Draw3DBox(Vector3 Position, Vector3 HeadPos, Vector3 FeetPos, Vector3 Rotation, ImColor BoxColor, float HeadHight, float FoodHight, bool CrosshairIn)
{
	float rot = acosf(Rotation.x) * 180.0f / DirectX::XM_PI;
	if (asinf(Rotation.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	ImVec2 t[8];
	Vector4 Cosines(0, 0, 0, 0);
	Vector4 Sines(0, 0, 0, 0);
	Vector4 Corners = Vector4(DirectX::XMConvertToRadians(rot - 45.0f), DirectX::XMConvertToRadians(rot - 135.0f), DirectX::XMConvertToRadians(rot + 45.0f), DirectX::XMConvertToRadians(rot + 135.0f));
	Cosines.x = cosf(Corners.x);
	Cosines.y = cosf(Corners.y);
	Cosines.z = cosf(Corners.z);
	Cosines.w = cosf(Corners.w);
	Sines.x = sinf(Corners.x);
	Sines.y = sinf(Corners.y);
	Sines.z = sinf(Corners.z);
	Sines.w = sinf(Corners.w);

	float radius = 0.5;

	Vector3 HeadLeftForward = Vector3(HeadPos.x + Cosines.x * radius, HeadPos.y + Sines.x * radius, HeadPos.z + 0.0f);
	Vector3 HeadLeftBackward = Vector3(HeadPos.x + Cosines.y * radius, HeadPos.y + Sines.y * radius, HeadPos.z + 0.0f);
	Vector3 HeadRightForward = Vector3(HeadPos.x + Cosines.z * radius, HeadPos.y + Sines.z * radius, HeadPos.z + 0.0f);
	Vector3 HeadRightBackward = Vector3(HeadPos.x + Cosines.w * radius, HeadPos.y + Sines.w * radius, HeadPos.z + 0.0f);

	Vector3 FeetLeftForward = Vector3(FeetPos.x + Cosines.x * radius, FeetPos.y + Sines.x * radius, FeetPos.z + 0.0f);
	Vector3 FeetLeftBackward = Vector3(FeetPos.x + Cosines.y * radius, FeetPos.y + Sines.y * radius, FeetPos.z + 0.0f);
	Vector3 FeetRightForward = Vector3(FeetPos.x + Cosines.z * radius, FeetPos.y + Sines.z * radius, FeetPos.z + 0.0f);
	Vector3 FeetRightBackward = Vector3(FeetPos.x + Cosines.w * radius, FeetPos.y + Sines.w * radius, FeetPos.z + 0.0f);

	t[0] = g_Math->WorldToScreen(HeadLeftForward);
	t[1] = g_Math->WorldToScreen(HeadLeftBackward);
	t[2] = g_Math->WorldToScreen(HeadRightForward);
	t[3] = g_Math->WorldToScreen(HeadRightBackward);

	t[4] = g_Math->WorldToScreen(FeetLeftForward);
	t[5] = g_Math->WorldToScreen(FeetLeftBackward);
	t[6] = g_Math->WorldToScreen(FeetRightForward);
	t[7] = g_Math->WorldToScreen(FeetRightBackward);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[1].x, t[1].y)))
		DrawLine({ t[0].x , t[0].y }, { t[1].x , t[1].y }, BoxColor);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[2].x, t[2].y)))
		DrawLine({ t[0].x , t[0].y }, { t[2].x , t[2].y }, BoxColor);

	if (isInScreen(ImVec2(t[1].x, t[1].y), ImVec2(t[3].x, t[3].y)))
		DrawLine({ t[1].x , t[1].y }, { t[3].x , t[3].y }, BoxColor);

	if (isInScreen(ImVec2(t[2].x, t[2].y), ImVec2(t[3].x, t[3].y)))
		DrawLine({ t[2].x , t[2].y }, { t[3].x , t[3].y }, BoxColor);

	if (isInScreen(ImVec2(t[4].x, t[4].y), ImVec2(t[5].x, t[5].y)))
		DrawLine({ t[4].x , t[4].y }, { t[5].x , t[5].y }, BoxColor);

	if (isInScreen(ImVec2(t[4].x, t[4].y), ImVec2(t[6].x, t[6].y)))
		DrawLine({ t[4].x , t[4].y }, { t[6].x , t[6].y }, BoxColor);

	if (isInScreen(ImVec2(t[5].x, t[5].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[5].x , t[5].y }, { t[7].x , t[7].y }, BoxColor);

	if (isInScreen(ImVec2(t[6].x, t[6].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[6].x , t[6].y }, { t[7].x , t[7].y }, BoxColor);

	if (isInScreen(ImVec2(t[0].x, t[0].y), ImVec2(t[4].x, t[4].y)))
		DrawLine({ t[0].x , t[0].y }, { t[4].x , t[4].y }, BoxColor);

	if (isInScreen(ImVec2(t[1].x, t[1].y), ImVec2(t[5].x, t[5].y)))
		DrawLine({ t[1].x , t[1].y }, { t[5].x , t[5].y }, BoxColor);

	if (isInScreen(ImVec2(t[2].x, t[2].y), ImVec2(t[6].x, t[6].y)))
		DrawLine({ t[2].x , t[2].y }, { t[6].x , t[6].y }, BoxColor);

	if (isInScreen(ImVec2(t[3].x, t[3].y), ImVec2(t[7].x, t[7].y)))
		DrawLine({ t[3].x , t[3].y }, { t[7].x , t[7].y }, BoxColor);

	return true;
}

void Renderer::DrawLine(ImVec2 src, ImVec2 dst, ImColor col, float thickness)
{

	if (this->isonscreen(dst) && this->isonscreen(src))
	{
		ImGui::GetBackgroundDrawList()->AddLine({ src.x, src.y }, { dst.x, dst.y }, col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ src.x + thickness, src.y + thickness }, { dst.x + thickness, dst.y + thickness }, ImColor(0, 0, 0), 0.5f);
	}
}
void Renderer::DrawLineV(ImVec2 src, ImVec2 dst, ImColor col, float thickness)
{

	if (this->isonscreen(dst) && this->isonscreen(src))
	{
		ImGui::GetBackgroundDrawList()->AddLine({ src.x - thickness/2, src.y - thickness/2 }, { dst.x, dst.y }, col, thickness);
		ImGui::GetBackgroundDrawList()->AddLine({ src.x + thickness/2, src.y + thickness/2 }, { dst.x, dst.y }, col * 0.75f, thickness);
	}
}
void Renderer::DrawBorder(float x, float y, float w, float h, float thickness, ImColor BorderColor)
{

	FillRect(x, y, w, thickness, BorderColor);

	FillRect(x, y, thickness, h, BorderColor);

	FillRect((x + w), y, thickness, h, BorderColor);

	FillRect(x, y + h, w + thickness, thickness, BorderColor);
}

void Renderer::FillRect(float x, float y, float w, float h, ImColor color, int rounding)
{
	if (this->isonscreen({ x, y }))
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);

	}
}

void Renderer::Rect(float x, float y, float w, float h, ImColor color, int rounding)
{
	if (this->isonscreen({ x, y }))
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);

	}
}

void Renderer::WindowFillRect(float x, float y, float w, float h, ImColor color, int rounding)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}
void Renderer::DrawCircle(float x, float y,float radius, ImColor color)
{
	if (this->isonscreen({ x, y }))
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(x, y), radius, color, 0);
}
 
bool Renderer::WindowDrawHealthBar(float x, float y, float w, float h, float health, float max, bool Side, ImColor col, bool border)
{
	if (!Side) {
		if (health > max)
			health = max;
			//return false;

		if (w < 5.0f)
			return false;

		if (health < 0.0f)
			health = 0.0f;

		float step = (w / max);

		if (step > max)
			return false;


		float draw = (step * health);


		/*if (border)
		{
								2
			DrawBorder2(x - 1, y - 1, w + 1, h + 1, 1, ImColor(0, 0, 0)); //Border
		}*/


		//DrawBorder(x - 1, y - 1, w + 1, h + 1, 1, ImColor(0, 0, 0));
		WindowFillRect(x, y, w, h, ImColor(0, 0, 0));
		WindowFillRect(x, y, draw, h, col); //Healthbar



	}
	else {
		if (health > max)
			return false;

		if (h < 5.0f)
			return false;

		if (health < 0.0f)
			health = 0.0f;

		float step = (h / max);

		if (step > max)
			return false;

		float draw = (step * health);

		FillRect(x, y, w, h, ImColor(0, 0, 0, 255), 2); //Background
		FillRect(x, y, w, draw, col, 2); //Healthbar
		//DrawBorder(x - 1.0f, y - 1.0f, w + 1.0f, h + 1.0f, 1.0f, ImColor(0, 0, 0, 255));


	}
	return true;
}