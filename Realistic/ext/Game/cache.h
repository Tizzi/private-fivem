#pragma once
#include <Includes.h>
class Cache
{
public:
	void Initialize();
};



struct cEntity
{

	INT32 index = 0;
	int id = 0;
	const char* Name;
	bool isFriend;
	bool isVisible;
	bool isOnscreen;
	bool isBestTarget;
	Vector3 relativePos = {0.f,0.f,0.f};
	Vector3 Head, Neck, Pelvis, RFoot, LFoot, LHand, RHand;
	cEntity() : Name(nullptr), index(0), id(0), relativePos({0,0,0}), Head({ 0,0,0 }), Neck({ 0,0,0 }), Pelvis({ 0,0,0 }), RFoot({ 0,0,0 }), LFoot({ 0,0,0 }), LHand({ 0,0,0 }), RHand({ 0,0,0 }) {}
};
struct
{
	std::vector<cEntity> entities;
}p_cached;
inline Cache* Cached = new Cache;