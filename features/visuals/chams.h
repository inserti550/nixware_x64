#pragma once
#include "../../hooks/hooks.h"

namespace chams
{
	i_material* create_material(const char* name, bool flat, bool wireframe, bool add_shine);

	__declspec(noinline) void push_material_override(float color[4], int material_type);
	__declspec(noinline) void pop_material_override();

	void push_ignore_z(bool ignore_z);
	void pop_ignore_z();
}