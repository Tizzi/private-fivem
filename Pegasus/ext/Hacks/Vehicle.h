#pragma once
#include <Includes.h>
#include <invoker.hpp>
class Vehicle_Func
{
public:
	void Initialize(struct timer timer);
	void DestroyVehicle(Vehicle veh);
};

inline Vehicle_Func* p_vehicle_func = new Vehicle_Func;
