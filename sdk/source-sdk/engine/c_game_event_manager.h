#pragma once

class c_game_event_manager
{
public:
	void add_listener(i_game_event_listener2* listener, const char* event, bool bIsServerSide)
	{
		return memory::call_v_function<void(__thiscall*)(void*, i_game_event_listener2*, const char*, bool)>(this, 3)(this, listener, event, bIsServerSide);
	}

	void find_listener(i_game_event_listener2* listener, const char* event)
	{
		return memory::call_v_function<void(__thiscall*)(void*, i_game_event_listener2*, const char*)>(this, 4)(this, listener, event);
	}

	void remove_listener(i_game_event_listener2* listener)
	{
		return memory::call_v_function<void(__thiscall*)(void*, i_game_event_listener2*)>(this, 5)(this, listener);
	}

	void reset()
	{
		return memory::call_v_function<void(__thiscall*)(void*)>(this, 2)(this);
	}

	void fire_event(i_game_event* event)
	{
		return memory::call_v_function<void(__thiscall*)(void*, i_game_event*)>(this, 7)(this, event);
	}
};