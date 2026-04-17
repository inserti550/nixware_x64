#pragma once
class i_game_event
{
public:
	const char* get_name()
	{
		return memory::call_v_function<const char*(__thiscall*)(void*)>(this, 1)(this);
	}
	bool is_reliable()
	{
		return memory::call_v_function<bool(__thiscall*)(void*)>(this, 2)(this);
	}
	bool is_local()
	{
		return memory::call_v_function<bool(__thiscall*)(void*)>(this, 3)(this);
	}
	bool is_empty(const char* keyname)
	{
		return memory::call_v_function<bool(__thiscall*)(void*, const char*)>(this, 4)(this, keyname);
	}
	bool get_bool(const char* keyname, bool default_value = false)
	{
		return memory::call_v_function<bool(__thiscall*)(void*, const char*, bool)>(this, 5)(this, keyname, default_value);
	}
	int get_int(const char* keyname, int default_value = 0)
	{
		return memory::call_v_function<int(__thiscall*)(void*, const char*, int)>(this, 6)(this, keyname, default_value);
	}
	float get_float(const char* keyname, float default_value = 0.f)
	{
		return memory::call_v_function<float(__thiscall*)(void*, const char*, float)>(this, 8)(this, keyname, default_value);
	}
	const char* get_string(const char* keyname, const char* default_value = "")
	{
		return memory::call_v_function<const char*(__thiscall*)(void*, const char*, const char*)>(this, 9)(this, keyname, default_value);
	}
	const wchar_t* get_wstring(const char* keyname, const wchar_t* default_value = L"")
	{
		return memory::call_v_function<const wchar_t* (__thiscall*)(void*, const char*, const wchar_t*)>(this, 10)(this, keyname, default_value);
	}
	const void* get_pointer(const char* keyname, const void* default_value = NULL)
	{
		return memory::call_v_function<const void*(__thiscall*)(void*, const char*, const void*)>(this, 11)(this, keyname, default_value);
	}
	void set_bool(const char* keyname, bool value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, bool)>(this, 12)(this, keyname, value);
	}
	void set_int(const char* keyname, int value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, int)>(this, 13)(this, keyname, value);
	}
	void set_float(const char* keyname, float value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, float)>(this, 15)(this, keyname, value);
	}
	void set_string(const char* keyname, const char* value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, const char*)>(this, 16)(this, keyname, value);
	}
	void set_wstring(const char* keyname, const wchar_t* value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, const wchar_t*)>(this, 17)(this, keyname, value);
	}
	void set_pointer(const char* keyname, const void* value)
	{
		return memory::call_v_function<void(__thiscall*)(void*, const char*, const void*)>(this, 18)(this, keyname, value);
	}
};