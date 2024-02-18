#pragma once
#include <Includes.h>
#include <invoker.hpp>
#include <Renderer.h>

class Radar3D
{
public:
	void Initialize();
};

class Attackers
{
	std::vector<int> Attackers;
public:
	bool isAttacker(int pedid)
	{
		auto it = std::find(Attackers.begin(), Attackers.end(), pedid);
		return (it != Attackers.end());
	}
	void add(int pedid)
	{
		auto it = std::find(Attackers.begin(), Attackers.end(), pedid);
		if (it == Attackers.end()) Attackers.push_back(pedid);
	}
	void remove(int pedid)
	{
		auto it = std::find(Attackers.begin(), Attackers.end(), pedid);
		if (it != Attackers.end()) Attackers.erase(it);

	}
};
inline Radar3D* p_radar = new Radar3D; 