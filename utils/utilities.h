#pragma once

#include "../sdk/interfaces.h"
#include "../settings.h"

namespace utilities
{

	c_vector to_vector(q_angle angle);
	c_vector side_vector(q_angle angle);
	c_vector up_vector(q_angle angle);
	std::string get_steam_id(int index);
	ImU32 Float4toImU32(float color[]);
	ImVec2 RotatePoint(c_vector EntityPos, c_vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck);
	bool get_entity_box_3d(c_base_entity* entity, dbox_t& box);

	bool game_is_full_loaded();
	void attach_console();
	void detach_console();
	void run_javascript(void* html_panel, const char* script);
	const char* host_cleanup_con_var_string_value(const char* str);
	std::vector<std::string> get_files_from_folder(const std::string& path, const std::string& search, const std::string& search_extension);
	std::string get_last_modified_time(const std::string& file_path);
	bool is_key_pressed(int key);
	void trace_line(c_vector start, c_vector end, unsigned int mask, c_base_entity* skip, trace_t* trace);
	int time_to_ticks(float time);
	float ticks_to_time(int ticks);
	float get_fov(const q_angle& from, const q_angle& to);
	bool screen_transform(const c_vector& in, c_vector* out);
	bool world_to_screen(const c_vector& in, c_vector* out);
	bool get_entity_box(c_base_entity* entity, box_t& box);

	void update_entity_list(nlohmann::json& list);
}