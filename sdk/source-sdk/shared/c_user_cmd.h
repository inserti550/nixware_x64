#pragma once

typedef unsigned int crc32_t;

class c_user_cmd
{
public:
    int             command_number;
    int             tick_count;
    q_angle         view_angles;

    float           forward_move;
    float           side_move;
    float           up_move;

    int             buttons;

    int             weapon_select;
    int             weapon_subtype;

    int             random_seed;

    short           mouse_x;
    short           mouse_y;

    bool            has_been_predicted;

    unsigned char   impulse;
    bool            is_typing;

    char            pad_0001[6];

    char            scroll_wheel_speed;
    bool            context_menu;

    c_vector        context_normal;

    char            pad_0002[240];

public:
    crc32_t get_check_sum()
    {
        using get_check_sum_fn = crc32_t(__fastcall*)(void*);
        static get_check_sum_fn get_check_sum =
            (get_check_sum_fn)memory::relative_to_absolute(
                (uintptr_t)memory::pattern_scanner(
                    xorstr("client.dll"),
                    xorstr("E8 ? ? ? ? 44 8B C8 B8 ? ? ? ?")),
                1, 5);

        if (!get_check_sum)
            throw;

        return get_check_sum(this);
    }
};

static_assert(sizeof(c_user_cmd) == 316);

class c_verified_user_cmd
{
public:
	void set_user_cmd(c_user_cmd* cmd)
	{
		m_cmd = *cmd;
		m_crc = cmd->get_check_sum();
	}

	c_user_cmd		m_cmd;
	crc32_t			m_crc;
};