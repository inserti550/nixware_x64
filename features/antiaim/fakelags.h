#pragma once
#include "../../hooks/hooks.h"

namespace fakelags
{
	static void* p_send_packet = nullptr;

	void init();
	bool run(c_user_cmd* cmd);
	void apply(bool send_packet);
}