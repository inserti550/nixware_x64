 #include "aimbot.h"
#include "penetrate_walls.h"
#include "backtrack/history.h"
#include "backtrack/lag_compensation.h"

void aimbot::smooth(c_user_cmd* cmd, q_angle& angle)
{
	if (!settings::aimbot::accuracy::smooth || settings::aimbot::globals::silent)
		return;

	angle.clamp();

	q_angle delta = angle - cmd->view_angles;
	delta.clamp();

	angle = cmd->view_angles + delta / settings::aimbot::accuracy::smooth;
}

bool aimbot::check_hitbox_group(int group) 
{
	switch (settings::aimbot::globals::hitbox) 
	{
	case 0: return group == HITGROUP_HEAD;
	case 1: return group == HITGROUP_CHEST;
	case 2: return group == HITGROUP_STOMACH;
	default: return true;
	}
}

bool aimbot::get_hit_position(c_base_entity* local_player, c_base_entity* entity, c_vector& shoot_pos, std::shared_ptr<matrix3x4[]> bone_to_world)
{
	void* model = entity->get_client_renderable()->get_model();
	if (!model)
		return false;

	studiohdr_t* hdr = interfaces::model_info->get_studio_model(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* hitbox_set = hdr->hitbox_set(entity->hitbox_set());
	if (!hitbox_set)
		return false;

	if (!bone_to_world)
	{
		bone_to_world = std::make_unique<matrix3x4[]>(hdr->num_bones);

		entity->invalidate_bone_cache();
		if (!entity->get_client_renderable()->setup_bones(bone_to_world.get(), hdr->num_bones, BONE_USED_BY_ANYTHING, interfaces::global_vars->curtime))
			return false;
	}

	bool found_hitbox = false;

	for (int i = 0; i < hitbox_set->num_hitboxes; i++)
	{
		mstudiobbox_t* hitbox = hitbox_set->hitbox(i);
		if (!hitbox)
			continue;

		if (!check_hitbox_group(hitbox->group))
			continue;

		c_vector mins, maxs;
		math::vector_transform(hitbox->bb_min, bone_to_world[hitbox->bone], mins);
		math::vector_transform(hitbox->bb_max, bone_to_world[hitbox->bone], maxs);
		shoot_pos = (mins + maxs) * 0.5;

		if (penetrate_walls::can_hit(local_player, entity, shoot_pos))
			return true;

		found_hitbox = true;
	}
	
	if (!found_hitbox)
	{
		c_vector pos = entity->get_abs_origin();
		c_vector mins = pos + entity->get_collidable()->mins();
		c_vector maxs = pos + entity->get_collidable()->maxs();
		shoot_pos = (mins + maxs) * 0.5;

		return penetrate_walls::can_hit(local_player, entity, shoot_pos);
	}

	return false;
}

target_info aimbot::find_best_target(c_user_cmd* cmd, c_base_entity* local_player)
{
	target_info target;
	priority_info& priority = target.priority_info;

	q_angle view_angles = cmd->view_angles;
	c_vector origin = local_player->get_abs_origin();

	c_vector eye_pos;
	local_player->get_eye_position(eye_pos);

	for (size_t i = 1; i <= interfaces::entity_list->get_highest_entity_index(); i++)
	{
		c_base_entity* entity = interfaces::entity_list->get_entity(i);

		if (!entity || !entity->is_player() || !entity->is_alive() || entity == local_player)
			continue;
		if (settings::aimbot::accuracy::friend_list.contains(lua_utilities::get_steam_id(entity)))
		{
			printf("ignore friend\n");
			continue;
		}
		float simulation_time = entity->get_simulation_time();
		int distance = origin.distance_to(entity->get_abs_origin());
		int health = entity->get_health();
		c_vector shoot_pos;

		if (!get_hit_position(local_player, entity, shoot_pos))
		{
			if (!settings::aimbot::accuracy::backtrack)
				continue;

			lag_record record;
			if (!history::get_latest_record(i, record))
				continue;

			if (!get_hit_position(local_player, entity, shoot_pos, record.bone_to_world))
				continue;

			simulation_time = record.simulation_time;
			distance = record.origin.distance_to(entity->get_abs_origin());
		}

		q_angle angle = math::calc_angle(globals::view_origin, shoot_pos);

		float fov = utilities::get_fov(globals::view_angles, angle);
		if (fov > settings::aimbot::globals::fov)
			continue;

		q_angle shoot_angle = math::calc_angle(eye_pos, shoot_pos);

		bool should_skip = false;
		switch (settings::aimbot::globals::priority)
		{
		case 0: should_skip = fov > priority.fov; break;
		case 1: should_skip = distance > priority.distance; break;
		case 2: should_skip = health > priority.health; break;
		}

		if (should_skip)
			continue;

		priority.fov = fov;
		priority.distance = distance;
		priority.health = health;

		target.entity = entity;
		target.shoot_pos = shoot_pos;
		target.shoot_angle = shoot_angle;
		target.simulation_time = simulation_time;
	}

	return target;
}

void aimbot::run(c_user_cmd* cmd)
{
	target = {};

	if (!settings::aimbot::globals::enable)
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
	if (!local_player || !local_player->is_alive())
		return;

	c_base_combat_weapon* weapon = local_player->get_active_weapon();
	if (!weapon || !weapon->can_fire() || weapon->is_holding_tool())
		return;

	target = find_best_target(cmd, local_player);
	if (!target.entity)
		return;

	if (settings::menu::opened)
		return;

	if (cmd->is_typing || cmd->is_world_clicking)
		return;

	if (!settings::aimbot::globals::hotkey.check())
		return;

	if (weapon->next_primary_attack() > interfaces::global_vars->curtime)
		return;

	if (lag_compensation::get_is_locked())
		return;

	smooth(cmd, target.shoot_angle);

	cmd->view_angles = target.shoot_angle;

	if (!settings::aimbot::globals::silent)
		interfaces::engine->set_view_angles(target.shoot_angle);

	if (settings::aimbot::globals::automatic_fire)
		cmd->buttons |= IN_ATTACK;

	bool adjust_interp;
	history::can_restore_to_simulation_time(target.simulation_time, &adjust_interp);

	if (adjust_interp)
	{
		float interp = utilities::ticks_to_time(interfaces::global_vars->tick_count) - target.simulation_time;
		if (interp > 0.f)
			lag_compensation::update_desired_values(true, interp, 1.f);
	}
}

void knifebot::smooth(c_user_cmd* cmd, q_angle& angle)
{
	if (!settings::aimbot::accuracy::smooth || settings::aimbot::globals::silent)
		return;

	angle.clamp();

	q_angle delta = angle - cmd->view_angles;
	delta.clamp();

	angle = cmd->view_angles + delta / settings::aimbot::accuracy::smooth;
}

bool knifebot::check_hitbox_group(int group)
{
	switch (settings::knifebot::globals::hitbox)
	{
	case 0: return group == HITGROUP_HEAD;
	case 1: return group == HITGROUP_CHEST;
	case 2: return group == HITGROUP_STOMACH;
	default: return true;
	}
}

bool knifebot::get_hit_position(c_base_entity* local_player, c_base_entity* entity, c_vector& shoot_pos, std::shared_ptr<matrix3x4[]> bone_to_world)
{
	void* model = entity->get_client_renderable()->get_model();
	if (!model)
		return false;

	studiohdr_t* hdr = interfaces::model_info->get_studio_model(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* hitbox_set = hdr->hitbox_set(entity->hitbox_set());
	if (!hitbox_set)
		return false;

	if (!bone_to_world)
	{
		bone_to_world = std::make_unique<matrix3x4[]>(hdr->num_bones);

		entity->invalidate_bone_cache();
		if (!entity->get_client_renderable()->setup_bones(bone_to_world.get(), hdr->num_bones, BONE_USED_BY_ANYTHING, interfaces::global_vars->curtime))
			return false;
	}

	bool found_hitbox = false;

	for (int i = 0; i < hitbox_set->num_hitboxes; i++)
	{
		mstudiobbox_t* hitbox = hitbox_set->hitbox(i);
		if (!hitbox)
			continue;

		if (!check_hitbox_group(hitbox->group))
			continue;

		c_vector mins, maxs;
		math::vector_transform(hitbox->bb_min, bone_to_world[hitbox->bone], mins);
		math::vector_transform(hitbox->bb_max, bone_to_world[hitbox->bone], maxs);
		shoot_pos = (mins + maxs) * 0.5;

		if (penetrate_walls::can_hit(local_player, entity, shoot_pos))
			return true;

		found_hitbox = true;
	}

	if (!found_hitbox)
	{
		c_vector pos = entity->get_abs_origin();
		c_vector mins = pos + entity->get_collidable()->mins();
		c_vector maxs = pos + entity->get_collidable()->maxs();
		shoot_pos = (mins + maxs) * 0.5;

		return penetrate_walls::can_hit(local_player, entity, shoot_pos);
	}

	return false;
}

target_info knifebot::find_best_target(c_user_cmd* cmd, c_base_entity* local_player)
{
	target_info target;
	priority_info& priority = target.priority_info;

	q_angle view_angles = cmd->view_angles;
	c_vector origin = local_player->get_abs_origin();

	c_vector eye_pos;
	local_player->get_eye_position(eye_pos);

	for (size_t i = 1; i <= interfaces::entity_list->get_highest_entity_index(); i++)
	{
		c_base_entity* entity = interfaces::entity_list->get_entity(i);

		if (!entity || !entity->is_player() || !entity->is_alive() || entity == local_player)
			continue;

		if (settings::aimbot::accuracy::friend_list.contains(lua_utilities::get_steam_id(entity)))
		{
			printf("ignore friend\n");
			continue;
		}
		float simulation_time = entity->get_simulation_time();
		int distance = origin.distance_to(entity->get_abs_origin());
		int health = entity->get_health();
		c_vector shoot_pos;

		if (!get_hit_position(local_player, entity, shoot_pos))
		{
			if (!settings::aimbot::accuracy::backtrack)
				continue;

			lag_record record;
			if (!history::get_latest_record(i, record))
				continue;

			if (!get_hit_position(local_player, entity, shoot_pos, record.bone_to_world))
				continue;

			simulation_time = record.simulation_time;
			distance = record.origin.distance_to(entity->get_abs_origin());
		}

		q_angle angle = math::calc_angle(globals::view_origin, shoot_pos);
		float fov = utilities::get_fov(globals::view_angles, angle);
		if (fov > settings::knifebot::globals::fov)
			continue;

		q_angle shoot_angle = math::calc_angle(eye_pos, shoot_pos);

		bool should_skip = false;
		switch (settings::knifebot::globals::priority)
		{
		case 0: should_skip = fov > priority.fov; break;
		case 1: should_skip = distance > priority.distance; break;
		case 2: should_skip = health > priority.health; break;
		}

		if (should_skip)
			continue;

		priority.fov = fov;
		priority.distance = distance;
		priority.health = health;

		target.entity = entity;
		target.shoot_pos = shoot_pos;
		target.shoot_angle = shoot_angle;
		target.simulation_time = simulation_time;
	}

	return target;
}

void knifebot::run(c_user_cmd* cmd)
{
	target = {};

	if (!settings::knifebot::globals::enable)
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
	if (!local_player || !local_player->is_alive())
		return;

	target = find_best_target(cmd, local_player);
	if (!target.entity)
		return;

	if (settings::menu::opened)
		return;

	if (cmd->is_typing || cmd->is_world_clicking)
		return;

	if (!settings::knifebot::globals::hotkey.check())
		return;

	if (target.entity->get_abs_origin().distance_to(local_player->get_abs_origin()) > settings::knifebot::globals::maxdistance)
		return;

	if (lag_compensation::get_is_locked())
		return;

	if (!settings::knifebot::globals::silent)
		interfaces::engine->set_view_angles(target.shoot_angle);
	else
		cmd->view_angles = target.shoot_angle;

	if (settings::knifebot::globals::automatic_fire) {
		cmd->buttons |= IN_ATTACK;
	}
	bool adjust_interp;
	history::can_restore_to_simulation_time(target.simulation_time, &adjust_interp);

	if (adjust_interp)
	{
		float interp = utilities::ticks_to_time(interfaces::global_vars->tick_count) - target.simulation_time;
		if (interp > 0.f)
			lag_compensation::update_desired_values(true, interp, 1.f);
	}
}

c_vector triggerbot::calc_end_point(c_vector origin, c_vector direction, float distance) {
	float pitchRad = direction.x * math::PI_D / 180.0f;
	float yawRad = direction.y * math::PI_D / 180.0f;
	float x = cos(pitchRad) * cos(yawRad);
	float y = cos(pitchRad) * sin(yawRad);
	float z = -sin(pitchRad);

	return c_vector(
		origin.x + x * distance,
		origin.y + y * distance,
		origin.z + z * distance
	);
}

bool triggerbot::can_shoot(c_base_entity* player)
{
	trace_t trace;
	c_vector position;
	player->get_eye_position(position);
	q_angle direction = player->eye_angles();
	utilities::trace_line(position, triggerbot::calc_end_point(position, direction, settings::TrigerBot::maxdistance), MASK_SHOT, player, &trace);
	if (trace.entity && trace.entity->is_player() && trace.entity->is_alive())
		return true;

	return false;
}

void triggerbot::run(c_user_cmd* cmd)
{
	if (!settings::TrigerBot::enable)
		return;

	if (!interfaces::engine->is_in_game())
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
	if (!local_player || !local_player->is_alive())
		return;

	c_base_combat_weapon* weapon = local_player->get_active_weapon();
	if (!weapon || !weapon->can_fire() || weapon->is_holding_tool())
		return;

	if (weapon->next_primary_attack() > interfaces::global_vars->curtime)
		return;

	if (settings::TrigerBot::hotkey.check())
	{
		if (can_shoot(local_player))
			cmd->buttons |= IN_ATTACK;
	}
}