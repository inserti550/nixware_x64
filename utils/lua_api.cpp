#include "lua_api.h"

namespace lua_api
{
    LUA_FUNCTION(klog) {
        const char* msg = lua->check_string(1);
        globals::kill_log.push_back(msg);
        return 0;
    }

    LUA_FUNCTION(print_console) {
        int args = lua->top();

        for (int i = 1; i <= args; i++) {
            if (!lua->is_type(i, object_type_t::string))
                continue;

            std::cout << lua->get_string(i);
        }
        return 0;
    }

    LUA_FUNCTION(print_console_ln) {
        int args = lua->top();

        for (int i = 1; i <= args; i++) {
            if (!lua->is_type(i, object_type_t::string))
                continue;

            std::cout << lua->get_string(i);
        }

        std::cout << '\n';
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
        if (!lua->is_type(1, object_type_t::entity))
        {
            lua->pop(2);
            return false;
        }
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

    LUA_FUNCTION(set_view_angles) {
        if (!lua->is_type(1, object_type_t::vector))
        {
            lua->pop(2);
            return 0;
        }
        c_vector vec = lua->get_vector(1);
        hooks::handles::originals::set_view_angles(interfaces::engine, vec);
        return 0;
    }

    LUA_FUNCTION(get_mouse_y) {
        
        lua->push_number(globals::cmd_ref->mouse_y);
        return 1;
    }

    LUA_FUNCTION(get_mouse_x) {

        lua->push_number(globals::cmd_ref->mouse_x);
        return 1;
    }

    LUA_FUNCTION(get_buttons) {

        lua->push_number(globals::cmd_ref->buttons);
        return 1;
    }

    LUA_FUNCTION(get_command_number) {

        lua->push_number(globals::cmd_ref->command_number);
        return 1;
    }

    LUA_FUNCTION(cmd_set_view_angles) {
        if (!lua->is_type(1, object_type_t::vector))
        {
            lua->pop(2);
            return 0;
        }
        c_vector vec = lua->get_vector(1);
        globals::cmd_ref->view_angles = vec;
        return 0;
    }

    LUA_FUNCTION(convar_get_int) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (!cvar) { lua->push_number(0); return 1; }
        lua->push_number(cvar->get_int());
        return 1;
    }

    LUA_FUNCTION(convar_get_float) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (!cvar) { lua->push_number(0.f); return 1; }
        lua->push_number(cvar->get_float());
        return 1;
    }

    LUA_FUNCTION(convar_get_string) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (!cvar) { lua->push_string(""); return 1; }
        lua->push_string(cvar->string_value);
        return 1;
    }

    LUA_FUNCTION(convar_set_int) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        int val = static_cast<int>(lua->check_number(2));
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (cvar) cvar->set_value(val);
        return 0;
    }

    LUA_FUNCTION(convar_set_float) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        float val = static_cast<float>(lua->check_number(2));
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (cvar) cvar->set_value(val);
        return 0;
    }

    LUA_FUNCTION(convar_set_string) {
        lua->push_string("__cvar_name");
        lua->get_field(1, "__cvar_name");
        const char* name = lua->get_string(-1);
        lua->pop(1);
        const char* val = lua->check_string(2);
        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (cvar) cvar->set_value(val);
        return 0;
    }

    LUA_FUNCTION(get_convar) {
        const char* name = lua->check_string(1);

        c_con_var* cvar = interfaces::cvar->find_var(name);
        if (!cvar) {
            lua->push_nil();
            return 1;
        }

        auto push_fn = [&](const char* name, c_lua_function func) {
            lua->push_c_function(func);
            lua->set_field(-2, name);
            };


        lua->create_table();

        push_fn(xorstr("get_int"), convar_get_int);
        push_fn(xorstr("get_float"), convar_get_float);
        push_fn(xorstr("get_string"), convar_get_string);
        push_fn(xorstr("set_int"), convar_set_int);
        push_fn(xorstr("set_float"), convar_set_float);
        push_fn(xorstr("set_string"), convar_set_string);

        lua->push_string(name);
        lua->set_field(-2, xorstr("__cvar_name"));

        return 1;
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
        push_fn(xorstr("print_console"), print_console);
        push_fn(xorstr("print_console_ln"), print_console_ln);
        push_fn(xorstr("time_to_tick"), time_to_tick);
        push_fn(xorstr("ticks_to_time"), ticks_to_time);
        push_fn(xorstr("get_entity_box"), get_entity_box);
        push_fn(xorstr("set_view_angles"), set_view_angles);
        push_fn(xorstr("get_convar"), get_convar);
        lua->create_table();
        push_fn(xorstr("get_mouse_y"), get_mouse_y);
        push_fn(xorstr("get_mouse_x"), get_mouse_x);
        push_fn(xorstr("get_buttons"), get_buttons);
        push_fn(xorstr("get_command_number"), get_command_number);
        push_fn(xorstr("cmd_set_view_angles"), cmd_set_view_angles);
        lua->set_field(-2, xorstr("cmd"));

        lua->set_field(-2, xorstr("nixware"));
        lua->pop(1);
    }
}