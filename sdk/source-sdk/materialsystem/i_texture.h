#pragma once

class i_texture
{
public:
	const char* get_name()
	{
		return memory::call_v_function<const char*(__thiscall*)(void*)>(this, 0)(this);
	}

	const char* get_actual_width()
	{
		return memory::call_v_function<const char* (__thiscall*)(void*)>(this, 3)(this);
	}
	const char* get_actual_height()
	{
		return memory::call_v_function<const char* (__thiscall*)(void*)>(this, 4)(this);
	}
};

struct texture_t {
	char pad[0xC];
	IDirect3DTexture9* d3d_texture;
};