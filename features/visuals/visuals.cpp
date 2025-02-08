#include "visuals.h"
#include "../aimbot/aimbot.h"

void visuals::render()
{
	if (!render_manager::draw_list)
		return;

	if (!interfaces::engine->is_in_game())
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
	if (!local_player)
		return;

	ImGuiIO& io = GetIO();

	render_manager::draw_list->_ResetForNewFrame();
	render_manager::draw_list->PushTextureID(io.Fonts->TexID);
	render_manager::draw_list->PushClipRectFullScreen();

	c_vector origin = local_player->get_abs_origin();

	for (size_t i = 0; i <= interfaces::entity_list->get_highest_entity_index(); i++)
	{
		c_base_entity* entity = interfaces::entity_list->get_entity(i);
		if (!entity)
			continue;

		if (entity->is_player())
		{
			if (!settings::visuals::esp::players::enable)
				continue;

			if (!entity->is_alive())
				continue;

			if (!settings::visuals::esp::players::dormant && entity->is_dormant())
				continue;

			if (entity == local_player)
				continue;

			box_t box;
			dbox_t dbox;
			if (!utilities::get_entity_box(entity, box))
				continue;
			if (settings::visuals::esp::players::boxtype == 2 || settings::visuals::esp::players::boxtype == 3)
			{
				if (!utilities::get_entity_box_3d(entity, dbox))
					continue;
			}

			float offset = 0;
			float distance = origin.distance_to(entity->get_abs_origin());

			float alpha = std::clamp((settings::visuals::esp::players::render_distance - distance) / 100.f, 0.f, 1.f);
			if (alpha <= 0.0f)
				continue;

			PushStyleVar(ImGuiStyleVar_Alpha, alpha);

			c_vector screen_points[8];
			if (settings::visuals::esp::players::box)
				switch (settings::visuals::esp::players::boxtype)
				{
				case 0:
					render_manager::box(box, settings::visuals::esp::players::colors::box, 1.f);
					break;
				case 1:
					render_manager::cornerbox(box, settings::visuals::esp::players::colors::box, 1.f, settings::visuals::esp::players::edgesize);
					break;
				case 2:
					for (int i = 0; i < 8; ++i)
					{
						if (!utilities::world_to_screen(dbox.points[i], &screen_points[i]))
							return;
					}
					render_manager::tdbox(screen_points, settings::visuals::esp::players::colors::box, 1.f);
					break;
				case 3:
					render_manager::filled_rect(ImVec2(box.left, box.top), ImVec2(box.right, box.bottom), settings::visuals::esp::players::colors::box, 0);
					break;
				}

			if (settings::visuals::esp::players::health::enable)
			{
				auto procent_ = float(entity->get_health()) / float(entity->get_maxhealth());
				auto height_delta = box.bottom - box.top;
				if (settings::visuals::esp::players::health::outline)
				{
					render_manager::line(ImVec2(box.right + 3, box.bottom), ImVec2(box.right + 3, box.top), settings::visuals::esp::players::health::colors::outline, 4);
				}
				if (settings::visuals::esp::players::health::back)
				{
					render_manager::line(ImVec2(box.right + 3, box.bottom), ImVec2(box.right + 3, box.top), settings::visuals::esp::players::health::colors::backward, 2);
				}
				procent_ <= 1.f
					? render_manager::line(ImVec2(box.right + 3, box.top), ImVec2(box.right + 3, box.top + (height_delta * procent_)), settings::visuals::esp::players::health::colors::forward, 2)
					: render_manager::line(ImVec2(box.right + 3, box.bottom), ImVec2(box.right + 3, box.top), settings::visuals::esp::players::health::colors::forward, 2);
				if (settings::visuals::esp::players::health::text)
					render_manager::text(ImVec2(box.right + 5, box.bottom), std::to_string(entity->get_health()).c_str(), settings::visuals::esp::players::health::colors::text);
			}

			if (settings::visuals::esp::players::distance)
			{
				std::string distance_text = std::to_string((int)distance) + "m";
				render_manager::render_text(box, distance_text.c_str(), settings::visuals::esp::players::colors::distance, offset);
			}

			c_base_combat_weapon* weapon = entity->get_active_weapon();
			if (weapon)
			{
				if (settings::visuals::esp::players::weapon_name)
					render_manager::render_text(box, lua_utilities::language_get_phrase(lua_utilities::get_weapon_print_name(weapon)), settings::visuals::esp::players::colors::weapon_name, offset);
			}

			if (settings::visuals::esp::players::user_group)
				render_manager::render_text(box, lua_utilities::get_user_group(entity), settings::visuals::esp::players::colors::user_group, offset);

			if (settings::visuals::esp::players::rp_team)
				render_manager::render_text(box, lua_utilities::get_team_name(entity), settings::visuals::esp::players::colors::rp_team, offset);

			if (settings::visuals::esp::players::name)
				render_manager::render_text(box, lua_utilities::get_name(entity), settings::visuals::esp::players::colors::name, offset);

			PopStyleVar();
		}
		else if (settings::visuals::esp::entity::enable)
		{
			if (!settings::visuals::esp::entity::dormant && entity->is_dormant())
				continue;

			std::string name = entity->get_class_name();
			if (name.empty())
				continue;

			if (!settings::visuals::esp::entity::list.contains(name) || !settings::visuals::esp::entity::list[name].get<bool>())
				continue;

			box_t box;
			if (!utilities::get_entity_box(entity, box))
				continue;

			float offset = 0;
			float distance = origin.distance_to(entity->get_abs_origin());

			float alpha = std::clamp((settings::visuals::esp::entity::render_distance - distance) / 100.f, 0.f, 1.f);
			if (alpha <= 0.0f)
				continue;

			PushStyleVar(ImGuiStyleVar_Alpha, alpha);

			if (settings::visuals::esp::entity::box)
				render_manager::box(box, settings::visuals::esp::entity::colors::box, 1.f);

			if (settings::visuals::esp::entity::distance)
			{
				std::string distance_text = std::to_string((int)distance) + "m";
				render_manager::render_text(box, distance_text.c_str(), settings::visuals::esp::entity::colors::distance, offset);
			}

			if (settings::visuals::esp::entity::name)
				render_manager::render_text(box, name.c_str(), settings::visuals::esp::entity::colors::name, offset);

			PopStyleVar();
		}
	}

	ImVec2 center(io.DisplaySize.x / 2, io.DisplaySize.y / 2);

	if (settings::aimbot::visuals::snaplines && aimbot::target.entity)
	{
		c_vector pos;
		if (utilities::world_to_screen(aimbot::target.shoot_pos, &pos))
			render_manager::line(center, ImVec2(pos.x, pos.y), settings::aimbot::visuals::colors::snaplines, 1.f);
	}

	if (settings::aimbot::visuals::fov)
	{
		float screen_fov = tanf(math::deg2rad(0.5f * globals::fov)) * ((io.DisplaySize.x / io.DisplaySize.y) / (4.f / 3.f));
		float radius = tanf(math::deg2rad(settings::aimbot::globals::fov * 0.5) * 2) / screen_fov * (io.DisplaySize.x * 0.5);

		render_manager::circle(center, radius, settings::aimbot::visuals::colors::fov, 100, 1.f);
	}
	if (settings::knifebot::visuals::fov)
	{
		float screen_fov = tanf(math::deg2rad(0.5f * globals::fov)) * ((io.DisplaySize.x / io.DisplaySize.y) / (4.f / 3.f));
		float radius = tanf(math::deg2rad(settings::knifebot::globals::fov * 0.5) * 2) / screen_fov * (io.DisplaySize.x * 0.5);

		render_manager::circle(center, radius, settings::knifebot::visuals::colors::fov, 100, 1.f);
	}
	if (settings::knifebot::visuals::snaplines && knifebot::target.entity)
	{
		c_vector pos;
		if (utilities::world_to_screen(knifebot::target.shoot_pos, &pos))
			render_manager::line(center, ImVec2(pos.x, pos.y), settings::knifebot::visuals::colors::snaplines, 1.f);
	}
	render_manager::draw_list->PopClipRect();
}


void visuals::renderMap()
{

	if (!settings::visuals::radar::enable)
		return;
	if (!interfaces::engine->is_in_game())
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
	if (!local_player)
		return;

	ImGuiIO& io = GetIO();
	ImGuiStyle& style = GetStyle();

	PushStyleColor(ImGuiCol_WindowBg, ImVec4(settings::menu::colors::window_bg[0], settings::menu::colors::window_bg[1], settings::menu::colors::window_bg[2], settings::menu::colors::window_bg[3]));
	PushStyleColor(ImGuiCol_PopupBg, ImVec4(settings::menu::colors::window_bg[0], settings::menu::colors::window_bg[1], settings::menu::colors::window_bg[2], settings::menu::colors::window_bg[3]));
	PushStyleColor(ImGuiCol_ChildBg, ImVec4(settings::menu::colors::child_bg[0], settings::menu::colors::child_bg[1], settings::menu::colors::child_bg[2], settings::menu::colors::child_bg[3]));
	PushStyleColor(ImGuiCol_Text, ImVec4(settings::menu::colors::text[0], settings::menu::colors::text[1], settings::menu::colors::text[2], settings::menu::colors::text[3]));
	PushStyleColor(ImGuiCol_TextHovered, ImVec4(settings::menu::colors::text_hovered[0], settings::menu::colors::text_hovered[1], settings::menu::colors::text_hovered[2], settings::menu::colors::text_hovered[3]));
	PushStyleColor(ImGuiCol_TextActive, ImVec4(settings::menu::colors::text_active[0], settings::menu::colors::text_active[1], settings::menu::colors::text_active[2], settings::menu::colors::text_active[3]));
	PushStyleColor(ImGuiCol_FrameBg, ImVec4(settings::menu::colors::frame_bg[0], settings::menu::colors::frame_bg[1], settings::menu::colors::frame_bg[2], settings::menu::colors::frame_bg[3]));
	PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(settings::menu::colors::frame_hovered_bg[0], settings::menu::colors::frame_hovered_bg[1], settings::menu::colors::frame_hovered_bg[2], settings::menu::colors::frame_hovered_bg[3]));
	PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(settings::menu::colors::frame_active_bg[0], settings::menu::colors::frame_active_bg[1], settings::menu::colors::frame_active_bg[2], settings::menu::colors::frame_active_bg[3]));

	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350, 350));

	SetNextWindowPos(settings::visuals::radar::position, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	SetNextWindowSize(ImVec2(350, 350), ImGuiCond_Once);

	if (!Begin(xorstr("Nixware radar"), NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
		return;

	settings::visuals::radar::position = ImGui::GetWindowPos();
	ImGui::SetWindowPos(settings::visuals::radar::position);
	ImVec2 siz = ImGui::GetWindowSize();
	ImVec2 pos = ImGui::GetWindowPos();
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
	if (!windowDrawList)
		return;

	if (settings::visuals::radar::background)
		windowDrawList->AddRectFilled(pos, ImVec2(pos.x + settings::visuals::radar::size.x, pos.y + settings::visuals::radar::size.y), GetColorU32(ImVec4(settings::visuals::radar::colors::background[0], settings::visuals::radar::colors::background[1], settings::visuals::radar::colors::background[2], settings::visuals::radar::colors::background[3])), 0.f);

	if (settings::visuals::radar::lines)
	{
		windowDrawList->AddLine(ImVec2(pos.x + settings::visuals::radar::size.x / 2, pos.y),
			ImVec2(pos.x + settings::visuals::radar::size.x / 2, pos.y + settings::visuals::radar::size.y),
			GetColorU32(ImVec4(settings::visuals::radar::colors::Line[0], settings::visuals::radar::colors::Line[1], settings::visuals::radar::colors::Line[2], settings::visuals::radar::colors::Line[3])), 1);
		windowDrawList->AddLine(ImVec2(pos.x, pos.y + settings::visuals::radar::size.y / 2),
			ImVec2(pos.x + settings::visuals::radar::size.x, pos.y + settings::visuals::radar::size.y / 2),
			GetColorU32(ImVec4(settings::visuals::radar::colors::Line[0], settings::visuals::radar::colors::Line[1], settings::visuals::radar::colors::Line[2], settings::visuals::radar::colors::Line[3])), 1);
	}

	if (local_player)
	{
		c_vector LocalPosition;
		local_player->get_eye_position(LocalPosition);
		q_angle angle;
		interfaces::engine->get_view_angles(angle);
		bool aye = false;
		if (settings::visuals::radar::players)
		{
			for (int i = 1; i <= interfaces::entity_list->get_highest_entity_index(); i++)
			{
				c_base_entity* entity = interfaces::entity_list->get_entity(i);

				if (!entity || !entity->is_player() || !entity->is_alive() || entity == local_player)
					continue;

				ImVec2 EntityPos = utilities::RotatePoint(entity->get_abs_origin(), LocalPosition, pos.x, pos.y, siz.x, siz.y, angle.y, settings::visuals::radar::zoom, &aye);
				auto sid = utilities::get_steam_id(i);
				windowDrawList->AddCircleFilled(EntityPos, settings::visuals::radar::zoom * 2,
					settings::aimbot::accuracy::friend_list.contains(sid)
					? utilities::Float4toImU32(settings::visuals::radar::colors::Friends)
					: utilities::Float4toImU32(settings::visuals::radar::colors::Players), 6);

			}
		}
	}

	End();

	PopStyleColor(9);
	PopStyleVar();
}