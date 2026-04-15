#pragma once
#include "../../hooks/hooks.h"

namespace miscellaneous
{
	static c_vector cam_pos;
	static q_angle cam_ang;
	static bool freecam_state = false;
	void run();
	void third_person(c_view_setup& view);
	namespace freecam
	{
		void freecam(c_view_setup& view);
		void block(c_user_cmd* cmd);
	}
	void fullbright();
}