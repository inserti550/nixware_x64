#include "lua_api.h"

namespace lua_api
{
    LUA_FUNCTION(klog) {
        const char* msg = lua->check_string(1);
        globals::kill_log.push_back(msg);
        return 0;
    }

    LUA_FUNCTION(attach_console) {
		utilities::attach_console();
        return 0;
    }

    LUA_FUNCTION(detach_console) {
		utilities::detach_console();
        return 0;
    }

    LUA_FUNCTION(time_to_tick) {
        double time = lua->check_number(1);
        lua->push_number(utilities::time_to_ticks(static_cast<float>(time)));
        return 1;
    }

    LUA_FUNCTION(ticks_to_time) {
        int tick = static_cast<int>(lua->check_number(1));
        lua->push_number(utilities::ticks_to_time(tick));
        return 1;
    }

    LUA_FUNCTION(get_entity_box) {
        lua->check_type(1, lua::type::Entity);
        lua->push_string("EntIndex");
        lua->get_field(1, "EntIndex");
        lua->push(1);
        lua->call(1, 1);

        int index = static_cast<int>(lua->get_number(-1));
        lua->pop(1);
        c_base_entity* player = interfaces::entity_list->get_entity(index);
        if (!player) return 0;

        box_t box;
        if (!utilities::get_entity_box(player, box)) {
            lua->push_number(0);
            return 1;
        }

        lua->push_number(box.left);
        lua->push_number(box.top);
        lua->push_number(box.right);
        lua->push_number(box.bottom);
        return 4;
    }

    void initialize() {
        c_lua_interface* lua = interfaces::lua_shared->get_interface(lua_type_client);
        if (!lua) return;

        auto push_fn = [&](const char* name, c_lua_function func) {
            lua->push_c_function(func);
            lua->set_field(-2, name);
            };

        lua->push_special(lua_special_glob);
        lua->create_table();

        push_fn(xorstr("klog"), klog);
        push_fn(xorstr("attach_console"), attach_console);
        push_fn(xorstr("detach_console"), detach_console);
		push_fn(xorstr("time_to_tick"), time_to_tick);
        push_fn(xorstr("ticks_to_time"), ticks_to_time);
        push_fn(xorstr("get_entity_box"), get_entity_box);

        lua->set_field(-2, xorstr("nixware"));
        lua->pop(1);
    }
}