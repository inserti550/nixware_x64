#pragma once
#include "sdk/interfaces.h"

namespace globals
{
    inline bool unload = false;

    inline v_panel mat_system_top_panel = false;
    inline v_panel overlay_popup_panel = false;
    inline v_panel focus_overlay_panel = false;

    inline D3DMATRIX world_to_screen;

    inline c_vector view_origin;
    inline q_angle view_angles;
    inline float fov = 0;

    inline void* menu_panel = nullptr;
    inline std::atomic<std::pair<bool, const char*>> waiting_to_be_executed;

    inline c_user_cmd last_cmd;
    inline c_user_cmd last_real_cmd;
    inline c_user_cmd* cmd_ref;

	inline i_game_event_listener2* damageEvent;
	inline i_game_event_listener2* deathEvent;

    inline std::deque<std::string> kill_log;

	inline bool lua_initialized = false;
}

namespace settings
{
    namespace menu
    {
        inline bool opened = false;

        namespace custom_loading_screen
        {
            inline bool enable = true;
            inline char url[256] = "https://pa1n-dev.github.io/nixware/index.html";
        }

        namespace colors
        {
            inline float window_bg[4] = { 0.07f, 0.07f, 0.07f, 1.f };
            inline float child_bg[4] = { 0.1f, 0.1f, 0.1f, 1.f };
            inline float text[4] = { 0.4f, 0.4f, 0.4f, 1.f };
            inline float text_hovered[4] = { 0.8f, 0.8f, 0.8f, 1.f };
            inline float text_active[4] = { 1.f, 1.f, 1.f, 1.f };
            inline float frame_bg[4] = { 0.2f, 0.2f, 0.2f, 1.f };
            inline float frame_hovered_bg[4] = { 0.4f, 0.4f, 0.4f, 1.f };
            inline float frame_active_bg[4] = { 1.f, 1.f, 1.f, 1.f };
        }
    }

    namespace aimbot
    {
        namespace globals
        {
            inline bool enable = false;
            inline hotkey_t hotkey;
            inline bool silent = false;
            inline bool automatic_fire = false;
            inline bool penetrate_walls = false;
            inline float fov = 0;
            inline int hitbox = 0;
            inline int priority = 0;
        }

        namespace accuracy
        {
            inline bool predict_spread = false;
            inline bool disable_recoil = false;
            inline bool disable_visual_recoil = false;
            inline float backtrack = 0.f;
            inline float smooth = 0.f;
            inline nlohmann::json team_list;
            inline nlohmann::json friend_list;
        }

        namespace visuals
        {
            inline bool fov = false;
            inline bool snaplines = false;

            namespace backtrack
            {
                inline bool enable = false;
                inline int material_type = 0;
                inline int factor = 100;
            }

            namespace colors
            {
                inline float fov[4] = { 1.f, 1.f, 1.f, 1.f };
                inline float snaplines[4] = { 1.f, 1.f, 1.f, 1.f };
                inline float backtrack[4] = { 1.f, 1.f, 1.f, 1.f };
            }
        }
    }

    namespace antihit
    {
        namespace fake_lags
        {
            inline bool enable = false;
            inline int count = 1;
            inline int method = 0;
        }

        namespace fake_angles
        {
            inline bool enable = false;
            inline hotkey_t hotkey;
            inline int yaw = 0;
            inline int pitch = 0;
            inline bool at_target = false;
            inline bool invert_yaw = false;
            inline bool fake_duck = false;
        }

        namespace visuals
        {
            namespace fake_model
            {
                inline bool enable = false;
                inline int material_type = 0;
                inline int factor = 100;
            }

            namespace colors
            {
                inline float fake_model[4] = { 1.f, 1.f, 1.f, 1.f };
            }
        }
    }

    namespace visuals
    {
        namespace esp
        {
            namespace players
            {
                inline bool enable = false;
                inline bool dormant = false;
                inline bool box = false;
                inline bool name = false;
                inline bool rp_team = false;
                inline bool user_group = false;
                inline bool weapon_name = false;
                inline bool distance = false;
                inline int render_distance = 15000;

                namespace colors
                {
                    inline float box[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float name[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float rp_team[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float user_group[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float weapon_name[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float distance[4] = { 1.f, 1.f, 1.f, 1.f };
                }
            }

            namespace entity
            {
                inline bool enable = false;
                inline bool dormant = false;
                inline bool box = false;
                inline bool name = false;
                inline bool distance = false;
                inline int render_distance = 15000;

                inline nlohmann::json list;

                namespace colors
                {
                    inline float box[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float name[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float distance[4] = { 1.f, 1.f, 1.f, 1.f };
                }
            }
        }

        namespace chams
        {
            namespace players
            {
                inline bool enable = false;
                inline bool ignore_walls = false;
                inline int material_type = 0;
                inline int factor = 100;
                inline bool draw_original_model = false;
                
            }

            namespace entity
            {
                inline bool enable = false;
                inline bool ignore_walls = false;
                inline int material_type = 0;
                inline int factor = 100;
                inline bool draw_original_model = false;

                inline nlohmann::json list;
            }

            namespace hands
            {
                inline bool enable = false;
                inline int material_type = 0;
                inline int factor = 100;
                inline bool draw_original_model = false;
            }

            namespace colors
            {
                inline float players[4] = { 1.f, 1.f, 1.f, 1.f };
                inline float entity[4] = { 1.f, 1.f, 1.f, 1.f };
                inline float hands[4] = { 1.f, 1.f, 1.f, 1.f };
            }
        }

        namespace world
        {
            namespace fov_changer
            {
                inline bool enable = false;
                inline float value = 80;
            }

            namespace model_fov_changer
            {
                inline bool enable = false;
                inline float value = 80;
            }
        }
    }

    namespace miscellaneous
    {
        namespace globals
        {
            namespace third_person
            {
                inline bool enable = false;
                inline hotkey_t hotkey;
                inline int distance = 100;
            }
            namespace freecam
            {
                inline bool enable = false;
                inline hotkey_t hotkey;
                inline int speed = 25;
                inline bool saveposition = false;
            }
            namespace logs
            {
                inline bool enable = false;
                inline int lines = 10;
                namespace colors
                {
                    inline bool open = false;
                    inline float u_death[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float u_kill[4] = { 1.f, 1.f, 1.f, 1.f };
				    inline float k_death[4] = { 1.f, 1.f, 1.f, 1.f };
					inline float k_kill[4] = { 1.f, 1.f, 1.f, 1.f };
					inline float u_damage[4] = { 1.f, 1.f, 1.f, 1.f };
					inline float k_damage[4] = { 1.f, 1.f, 1.f, 1.f };
					inline float other[4] = { 1.f, 1.f, 1.f, 1.f };
                }
            }
            namespace indicators
            {
                namespace hitmarker
                {
                    inline bool enable = false;
                    inline float size = 10.f;
                    inline float color[4] = { 1.f, 1.f, 1.f, 1.f };
                    inline float last_hit_marker = -999.f;
                }
                namespace hitsound
                {
                    inline bool enable = false;
					inline int selected = 0;
                }
                namespace killsound
                {
                    inline bool enable = false;
                    inline int selected = 0;
                }
            }
            inline bool fullbright = false;
            inline bool luaapi = false;
        }

        namespace movement
        {
            inline bool bhop = false;
            inline bool air_strafe = false;

        }
    }

    namespace lua
    {
        namespace miscellaneous
        {
            inline bool dumper = false;
            inline bool net_logger = false;
        }
    }
}