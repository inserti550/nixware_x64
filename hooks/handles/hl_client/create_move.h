void __fastcall hooks::handles::create_move(c_hl_client* client, int sequence_number, float input_sample_frametime, bool active)
{
	originals::create_move(client, sequence_number, input_sample_frametime, active);

	c_user_cmd* cmd = interfaces::input->get_user_cmd(sequence_number);
	c_verified_user_cmd* verified_cmd = interfaces::input->get_verified_user_cmd(sequence_number);

	if (!cmd || !cmd->command_number)
		return;

	globals::last_cmd = *cmd;

	if (globals::is_in_freecam) {
		cmd->buttons = NULL;
		cmd->forward_move = cmd->side_move = cmd->up_move = 0.f;
		cmd->view_angles = globals::last_real_angle;
	}
	else
		globals::last_real_angle = cmd->view_angles;

	movement::run(cmd);

	c_user_cmd old_cmd = *cmd;

	engine_prediction::start(cmd);
	triggerbot::run(cmd);
	knifebot::run(cmd);
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