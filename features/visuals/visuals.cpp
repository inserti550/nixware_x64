#include "visuals.h"

void visuals::render()
{
	using namespace ImGui;

	if (!interfaces::engine->is_in_game())
		return;

	c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());

	ImGuiIO& io = GetIO();
	SetNextWindowPos(ImVec2(0, 0));
	SetNextWindowSize(io.DisplaySize);
	if (Begin(xorstr("Visuals"), NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground))
	{
		for (size_t i = 0; i < interfaces::entity_list->get_highest_entity_index(); i++)
		{
			c_base_entity* entity = interfaces::entity_list->get_entity(i);
			if (!entity)
				continue;

			if (entity->is_player())
			{
				if (!settings::visuals::players::enable)
					continue;

				if (!entity->is_alive())
					continue;

				if (!settings::visuals::players::dormant && entity->is_dormant())
					continue;

				if (entity == local_player)
					continue;

				box_t box{};
				if (!utilities::get_entity_box(entity, box))
					continue;

				if (settings::visuals::players::box)
				{
					render_manager::line(ImVec2(box.left, box.bottom), ImVec2(box.left, box.top), settings::visuals::players::colors::box, 1.f);
					render_manager::line(ImVec2(box.left, box.top), ImVec2(box.right, box.top), settings::visuals::players::colors::box, 1.f);
					render_manager::line(ImVec2(box.right, box.top), ImVec2(box.right, box.bottom), settings::visuals::players::colors::box, 1.f);
					render_manager::line(ImVec2(box.right, box.bottom), ImVec2(box.left, box.bottom), settings::visuals::players::colors::box, 1.f);
				}
			}
			else
			{
				if (!settings::visuals::entity::enable)
					continue;
			}
		}

		End();
	}
}