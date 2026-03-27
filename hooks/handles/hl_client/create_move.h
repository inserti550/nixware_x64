void __fastcall hooks::handles::create_move(c_hl_client* client, int sequence_number, float input_sample_frametime, bool active)
{
	originals::create_move(client, sequence_number, input_sample_frametime, active);

	c_user_cmd* cmd = interfaces::input->get_user_cmd(sequence_number);
	globals::last_cmd = *cmd;
	c_verified_user_cmd* verified_cmd = interfaces::input->get_verified_user_cmd(sequence_number);

	if (!cmd || !cmd->command_number)
		return;
	movement::run(cmd);

	if (!(settings::miscellaneous::globals::freecam::enable && settings::miscellaneous::globals::freecam::hotkey.check())) {
		cmd->view_angles = globals::last_cmd.view_angles;
		globals::last_real_cmd = *cmd;
	}
	else {
		cmd->buttons &= ~(IN_ATTACK | IN_ATTACK2 | IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_JUMP | IN_DUCK);
		cmd->forward_move = 0.f;
		cmd->side_move = 0.f;
		cmd->up_move = 0.f;

		cmd->view_angles = globals::last_real_cmd.view_angles;
	}

	c_user_cmd old_cmd = *cmd;
	engine_prediction::start(cmd);
	aimbot::run(cmd);
	engine_prediction::end(cmd);
	predict_spread::run(cmd);

	bool send_packet = fakelags::run(cmd);
	antiaim::run(cmd, send_packet);
	animfix::run(cmd, send_packet);
	fakelags::apply(send_packet);

	movement::fix(cmd, old_cmd);

	verified_cmd->set_user_cmd(cmd);

	lag_compensation::write_user_cmd_delta_to_buffer_callback();
}