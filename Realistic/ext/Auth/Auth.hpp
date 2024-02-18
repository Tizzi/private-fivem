#pragma once
#include <Includes.h>

class Auth
{
public:
 	std::string __domain = _xor_("http://45.131.111.84/").c_str();
	bool __getserver(std::string hwid); 
};

inline Auth* g_auth = new Auth; 