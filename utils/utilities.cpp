#include "utilities.h"

std::string utilities::get_steam_id(int id) {
	player_info_s info;
	interfaces::engine->get_player_info(id, &info);
	return info.guid;
}

c_vector utilities::to_vector(q_angle angle)
{
	return c_vector(std::cos(math::deg2rad(angle.x)) * std::cos(math::deg2rad(angle.y)), std::cos(math::deg2rad(angle.x)) * std::sin(math::deg2rad(angle.y)), -std::sin(math::deg2rad(angle.x)));
};
c_vector utilities::side_vector(q_angle angle)
{
	return c_vector(std::cos(math::deg2rad(angle.y + 90.f)), std::sin(math::deg2rad(angle.y + 90.f)), 0.f);
};
c_vector utilities::up_vector(q_angle angle)
{
	return c_vector(std::cos(math::deg2rad(angle.x + 90.f)) * std::cos(math::deg2rad(angle.y)), std::cos(math::deg2rad(angle.x - 90.f)) * std::sin(math::deg2rad(angle.y)), -std::sin(math::deg2rad(angle.x - 90.f)));
};

ImVec2 utilities::RotatePoint(c_vector EntityPos, c_vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;
	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;
	float yawToRadian = Yaw * (float)(math::PI_F / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;
	*viewCheck = y_1 < 0;
	x_1 *= zoom;
	y_1 *= zoom;
	int sizX = sizeX / 2;
	int sizY = sizeY / 2;
	x_1 += sizX;
	y_1 += sizY;
	if (x_1 < 5)
		x_1 = 5;
	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;
	if (y_1 < 5)
		y_1 = 5;
	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;
	x_1 += posX;
	y_1 += posY;
	return ImVec2(x_1, y_1);
}

ImU32 utilities::Float4toImU32(float color[])
{
	return ImGui::GetColorU32(ImVec4(color[0], color[1], color[2], color[3]));
}

bool utilities::get_entity_box_3d(c_base_entity* entity, dbox_t& box)
{
	c_vector pos = entity->get_abs_origin();
	c_vector mins = pos + entity->get_collidable()->mins();
	c_vector maxs = pos + entity->get_collidable()->maxs();

	box.points[0] = c_vector(mins.x, mins.y, mins.z);
	box.points[1] = c_vector(mins.x, maxs.y, mins.z);
	box.points[2] = c_vector(maxs.x, maxs.y, mins.z);
	box.points[3] = c_vector(maxs.x, mins.y, mins.z);
	box.points[4] = c_vector(maxs.x, maxs.y, maxs.z);
	box.points[5] = c_vector(mins.x, maxs.y, maxs.z);
	box.points[6] = c_vector(mins.x, mins.y, maxs.z);
	box.points[7] = c_vector(maxs.x, mins.y, maxs.z);

	for (int i = 0; i < 8; i++)
	{
		if (isnan(box.points[i].x) || isnan(box.points[i].y) || isnan(box.points[i].z))
			return false;
	}

	return true;
}

bool utilities::game_is_full_loaded()
{
	return GetModuleHandleA(xorstr("client.dll"))
		&& GetModuleHandleA(xorstr("engine.dll"))
		&& GetModuleHandleA(xorstr("menusystem.dll"))
		&& GetModuleHandleA(xorstr("gameoverlayrenderer64.dll"))
		&& GetModuleHandleA(xorstr("materialsystem.dll"))
		&& GetModuleHandleA(xorstr("lua_shared.dll"))
		&& GetModuleHandleA(xorstr("vstdlib.dll"))
		&& GetModuleHandleA(xorstr("vgui2.dll"));
}


void utilities::attach_console()
{
	AllocConsole();
	freopen(xorstr("conout$"), "w", stdout);
}

void utilities::detach_console()
{
	fclose(stdout);
	FreeConsole();
}

void utilities::run_javascript(void* html_panel, const char* script)
{
	using run_javascript_fn = void(*)(void*, const char*);
	static run_javascript_fn run_javascript = (run_javascript_fn)memory::pattern_scanner(xorstr("menusystem.dll"), xorstr("48 8B 89 ? ? ? ? 48 8B 01 48 FF 60 68"));

	if (!run_javascript)
		throw;

	if (html_panel)
		run_javascript(html_panel, script);
}

//https://media.discordapp.net/attachments/1194221664339234816/1214847464277024768/image.png?ex=65fa99de&is=65e824de&hm=0d881e116dd3a29aaf32d926899891343d2fce9aa895e54b0c865aa28eaaad6d&=&format=webp&quality=lossless
const char* utilities::host_cleanup_con_var_string_value(const char* str)
{
	using host_cleanup_con_var_string_value_fn = const char* (*)(const char*);
	static host_cleanup_con_var_string_value_fn host_cleanup_con_var_string_value = (host_cleanup_con_var_string_value_fn)memory::relative_to_absolute((uintptr_t)memory::pattern_scanner(xorstr("engine.dll"), xorstr("E8 ? ? ? ? 48 8B 16 48 8B CE 48 8B F8")), 1, 6);

	if (!host_cleanup_con_var_string_value)
		throw;

	return host_cleanup_con_var_string_value(str);
}

std::vector<std::string> utilities::get_files_from_folder(const std::string& path, const std::string& search, const std::string& search_extension)
{
	std::vector<std::string> list;

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
		std::filesystem::create_directories(path);

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string name = entry.path().stem().string();
		std::string extension = entry.path().extension().string();

		if (extension != search_extension)
			continue;

		if (search.empty() || name.find(search) != std::string::npos)
			list.push_back(name);
	}

	return list;
}

std::string utilities::get_last_modified_time(const std::string& file_path)
{
	auto ftime = std::filesystem::last_write_time(file_path);
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
	std::tm* ptm = std::localtime(&cftime);

	char buffer[32];
	std::strftime(buffer, sizeof(buffer), xorstr("%d %b %Y %H:%M"), ptm);

	return buffer;
}

bool utilities::is_key_pressed(int key)
{
	static bool key_press[256];

	if (GetAsyncKeyState(key) & 0x8000)
	{
		if (!key_press[key])
		{
			key_press[key] = true;
			return true;
		}
	}
	else
		key_press[key] = false;

	return false;
}

void utilities::trace_line(c_vector start, c_vector end, unsigned int mask, c_base_entity* skip, trace_t* trace)
{
	ray_t ray(start, end);

	c_trace_filter filter;
	filter.skip = skip;

	interfaces::engine_trace->trace_ray(ray, mask, &filter, trace);
}

int utilities::time_to_ticks(float time)
{
	return (int)(0.5f + time / interfaces::global_vars->interval_per_tick);
}

float utilities::ticks_to_time(int ticks)
{
	return interfaces::global_vars->interval_per_tick * ticks;
}

float utilities::get_fov(const q_angle& from, const q_angle& to)
{
	c_vector from_forward, to_forward;

	math::angle_to_vector(from, from_forward);
	math::angle_to_vector(to, to_forward);

	return math::rad2deg(acosf(from_forward.dot(to_forward) / from_forward.length_sqr()));
}

bool utilities::screen_transform(const c_vector& in, c_vector* out)
{
	const D3DMATRIX& world_matrix = globals::world_to_screen;

	const auto w = world_matrix.m[3][0] * in.x + world_matrix.m[3][1] * in.y + world_matrix.m[3][2] * in.z + world_matrix.m[3][3];
	if (w < 0.001f)
	{
		out->x *= 100000;
		out->y *= 100000;
		return false;
	}

	out->x = world_matrix.m[0][0] * in.x + world_matrix.m[0][1] * in.y + world_matrix.m[0][2] * in.z + world_matrix.m[0][3];
	out->y = world_matrix.m[1][0] * in.x + world_matrix.m[1][1] * in.y + world_matrix.m[1][2] * in.z + world_matrix.m[1][3];
	out->z = 0.0f;

	out->x /= w;
	out->y /= w;

	return true;
}

bool utilities::world_to_screen(const c_vector& in, c_vector* out)
{
	if (!screen_transform(in, out))
		return false;

	int w, h;
	interfaces::engine->get_screen_size(w, h);

	out->x = (w / 2.0f) + (out->x * w) / 2.0f;
	out->y = (h / 2.0f) - (out->y * h) / 2.0f;

	return true;
}

bool utilities::get_entity_box(c_base_entity* entity, box_t& box)
{
	c_vector pos = entity->get_abs_origin();
	c_vector mins = pos + entity->get_collidable()->mins();
	c_vector maxs = pos + entity->get_collidable()->maxs();

	c_vector points[] = { c_vector(mins.x, mins.y, mins.z),
						  c_vector(mins.x, maxs.y, mins.z),
						c_vector(maxs.x, maxs.y, mins.z),
						c_vector(maxs.x, mins.y, mins.z),
						c_vector(maxs.x, maxs.y, maxs.z),
						c_vector(mins.x, maxs.y, maxs.z),
						c_vector(mins.x, mins.y, maxs.z),
						c_vector(maxs.x, mins.y, maxs.z) };

	c_vector screen_points[8];

	for (int i = 0; i < 8; ++i)
	{
		if (!world_to_screen(points[i], &screen_points[i]))
			return false;
	}

	float left		= screen_points[0].x;
	float top		= screen_points[0].y;
	float right		= screen_points[0].x;
	float bottom	= screen_points[0].y;

	for (int i = 1; i < 8; ++i)
	{
		left	= min(left, screen_points[i].x);
		top		= max(top, screen_points[i].y);
		right	= max(right, screen_points[i].x);
		bottom	= min(bottom, screen_points[i].y);
	}

	if (isnan(left) || isnan(top) || isnan(right) || isnan(bottom))
		return false;

	box.left	= left;
	box.top		= top;
	box.right	= right;
	box.bottom	= bottom;

	return true;
}

void utilities::update_entity_list(nlohmann::json& list)
{
	if (!interfaces::engine->is_in_game())
		return;

	for (int i = 0; i <= interfaces::entity_list->get_highest_entity_index(); i++)
	{
		c_base_entity* entity = interfaces::entity_list->get_entity(i);
		if (!entity)
			continue;

		if (entity->is_player())
			continue;

		std::string name = entity->get_class_name();
		if (name.empty())
			continue;

		if (list.contains(name))
			continue;

		list.emplace(name, false);
	}
}