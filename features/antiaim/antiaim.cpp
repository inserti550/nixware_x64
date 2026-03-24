#include "antiaim.h"
#include "../aimbot/aimbot.h"

void antiaim::run(c_user_cmd* cmd, bool send_packet)
{
    /*
    std::cout << "[antiaim::run][TIME] " << interfaces::global_vars->absoluteframetime << " | " << interfaces::global_vars->curtime << " | " << interfaces::global_vars->framecount << std::endl;
    std::cout << "[antiaim::run][TIME] " << interfaces::global_vars->frametime << " | " << interfaces::global_vars->interpolation_amount << " | " << interfaces::global_vars->interval_per_tick << std::endl;
    std::cout << "[antiaim::run][TIME] " << interfaces::global_vars->max_clients << " | " << interfaces::global_vars->network_protocol << " | " << interfaces::global_vars->network_protocol << std::endl;
    std::cout << "[antiaim::run][TIME] " << interfaces::global_vars->realtime << " | " << interfaces::global_vars->remote_client << " | " << interfaces::global_vars->sim_ticks_this_frame << " | " << interfaces::global_vars->tick_count << std::endl;
    */
    if (!settings::antihit::fake_angles::enable)
        return;

    if (!settings::antihit::fake_angles::hotkey.check())
        return;

    c_base_entity* local_player = interfaces::entity_list->get_entity(interfaces::engine->get_local_player());
    if (!local_player || !local_player->is_alive())
        return;

    if (local_player->get_move_type() == MOVETYPE_NOCLIP || local_player->get_move_type() == MOVETYPE_LADDER)
        return;

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK)
        return;

    q_angle view;
    interfaces::engine->get_view_angles(view);

    if (settings::antihit::fake_angles::at_target && aimbot::target.entity)
        view = aimbot::target.shoot_angle;

    float pitch = 0.f;
    switch (settings::antihit::fake_angles::pitch)
    {
    case 0:
        pitch = 90.f;
        break;
    case 1:
        pitch = -90.f;
        break;
    }

    float yaw = 0.f;
    switch (settings::antihit::fake_angles::yaw)
    {
    case 0:
        yaw = send_packet ^ settings::antihit::fake_angles::invert_yaw ? (view.y - 90.f) : (view.y + 90.f);
        break;
    }

    cmd->view_angles.x = pitch;
    cmd->view_angles.y = yaw;
    cmd->view_angles.clamp();
}