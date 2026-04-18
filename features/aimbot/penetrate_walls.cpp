#include "penetrate_walls.h"

bool penetrate_walls::is_visible(c_base_entity* local_player, c_base_entity* entity, c_vector start, c_vector end)
{
	trace_t trace;

	utilities::trace_line(start, end, MASK_SHOT, local_player, &trace);

	return (trace.entity == entity || trace.fraction >= 0.98f);
}

bool penetrate_walls::m9k(c_base_entity* local_player, c_base_combat_weapon* weapon, c_base_entity* entity, c_vector eye_pos, c_vector shoot_pos)
{
	m9k_info info;
	if (!lua_utilities::build_m9k_info(weapon, info))
		return false;

	if (!info.penetration)
		return false;

	float damage = info.damage;
	int bounces = 0;

	c_vector current_start = eye_pos;
	c_vector current_end = shoot_pos;

	while (damage > 1) {
		trace_t trace;
		trace_t trace2;
		trace_t trace3;

		utilities::trace_line(current_start, current_end, MASK_SHOT, local_player, &trace);

		if (trace.fraction >= 1.0f || trace.fraction <= 0.0f)
			return false;

		surface_data_t* surface_data = interfaces::physics_surface_props->get_surface_data(trace.surface.props);

		if (surface_data->material == MAT_METAL && info.ricochet && info.ammo != "SniperPenetratedRound")
			return false;

		if (bounces > info.max_ricochet)
			return false;

		c_vector dir = (current_end - current_start).normalized() * info.max_penetration;
		if (surface_data->material == MAT_GLASS || surface_data->material == MAT_PLASTIC || surface_data->material == MAT_WOOD || surface_data->material == MAT_FLESH || surface_data->material == MAT_ALIENFLESH)
			dir = (current_end - current_start).normalized() * (info.max_penetration * 2);

		c_vector hit_pos = current_start + (current_end - current_start) * trace.fraction;

		c_vector start2 = hit_pos + dir;
		c_vector end2 = hit_pos;
		utilities::trace_line(start2, end2, MASK_SHOT, local_player, &trace2);

		if (trace2.start_solid || trace2.fraction >= 1.f)
			return false;

		hit_pos = start2 + (end2 - start2) * trace2.fraction;

		c_vector start3 = hit_pos;
		c_vector end3 = shoot_pos;

		utilities::trace_line(start3, end3, MASK_SHOT, local_player, &trace3);

		if (trace3.entity == entity)
			return true;

		float dmg_mult = 0.5f;
		if (info.ammo == "SniperPenetratedRound")
			dmg_mult = 1.f;
		else if (surface_data->material == MAT_CONCRETE || surface_data->material == MAT_METAL)
			dmg_mult = 0.3f;
		else if (surface_data->material == MAT_WOOD || surface_data->material == MAT_PLASTIC || surface_data->material == MAT_GLASS)
			dmg_mult = 0.8f;
		else if (surface_data->material == MAT_FLESH || surface_data->material == MAT_ALIENFLESH)
			dmg_mult = 0.9f;

		damage *= dmg_mult;

		current_start = hit_pos;

		if (interfaces::physics_surface_props->get_surface_data(trace3.surface.props)->material == MAT_GLASS)
			bounces -= 1;
	}

	return false;
}

bool penetrate_walls::swb(c_base_entity* local_player, c_base_combat_weapon* weapon, c_base_entity* entity, c_vector eye_pos, c_vector shoot_pos)
{
	auto get_penetration_material_interaction = [](unsigned short material)
	{
		double out = 1.0;
		switch (material)
		{
		case MAT_SAND:
			out = 0.5;
			break;
		case MAT_DIRT:
			out = 0.8;
			break;
		case MAT_METAL:
			out = 1.1;
			break;
		case MAT_TILE:
			out = 0.9;
			break;
		case MAT_WOOD:
			out = 1.2;
			break;
		default:
			out = 1.0;
		}

		return out;
	};

	swb_info info;
	if (!lua_utilities::build_swb_info(weapon, info))
		return false;

	DWORD normal_trace_mask = CONTENTS_SOLID | CONTENTS_OPAQUE | CONTENTS_MOVEABLE | CONTENTS_DEBRIS | CONTENTS_MONSTER | CONTENTS_HITBOX | 402653442 | CONTENTS_WATER;
	DWORD wall_trace_mask = CONTENTS_TESTFOGVOLUME | CONTENTS_EMPTY | CONTENTS_MONSTER | CONTENTS_HITBOX;

	c_vector start = eye_pos;
	c_vector end = shoot_pos;
	trace_t trace;

	utilities::trace_line(start, end, normal_trace_mask, local_player, &trace);
	surface_data_t* surface_data = interfaces::physics_surface_props->get_surface_data(trace.surface.props);

	c_vector dir = (end - start).normalized();

	if (trace.entity && trace.entity == entity)
		return true;

	if (surface_data->material == MAT_SLOSH)
		return false;

	double dot_reflection = -dir.dot(trace.plane.normal);
	if (dot_reflection <= 0.26)
		return false;

	c_vector hit_pos = start + (end - start) * trace.fraction;
	start = hit_pos;
	end = start + dir * info.pen_str * get_penetration_material_interaction(surface_data->material) * info.pen_mod;
	utilities::trace_line(start, end, wall_trace_mask, local_player, &trace);

	hit_pos = start + (end - start) * trace.fraction;
	start = hit_pos;
	end = start + dir * 0.1;
	utilities::trace_line(start, end, normal_trace_mask, local_player, &trace);

	hit_pos = start + (end - start) * trace.fraction;
	start = hit_pos;
	end = shoot_pos;
	utilities::trace_line(start, end, MASK_SHOT, local_player, &trace);

	if (trace.entity && trace.entity == entity)
		return true;

	return false;
}

bool penetrate_walls::can_hit(c_base_entity* local_player, c_base_entity* entity, c_vector shoot_pos)
{
	c_vector eye_pos;
	local_player->get_eye_position(eye_pos);

	if (is_visible(local_player, entity, eye_pos, shoot_pos))
		return true;

	c_base_combat_weapon* weapon = local_player->get_active_weapon();
	if (!weapon)
		return false;

	if (settings::aimbot::globals::penetrate_walls)
	{
		const char* weapon_base = lua_utilities::get_weapon_base(weapon);

		if (strstr(weapon_base, xorstr("swb_")) || strstr(weapon_base, xorstr("cw_")))
			return swb(local_player, weapon, entity, eye_pos, shoot_pos);
		if (strstr(weapon_base, xorstr("bobs_")))
			return m9k(local_player, weapon, entity, eye_pos, shoot_pos);
	}

	return false;
}