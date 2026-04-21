#pragma once
class mat_system_surface
{
public:
	void* play_sound(const char* sound)
	{
		return memory::call_v_function<void* (__thiscall*)(void*, const char*)>(this, 83)(this, sound);
	}
};