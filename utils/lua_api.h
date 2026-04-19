#pragma once
#include "../sdk/interfaces.h"
#include "../hooks/hooks.h"

namespace lua_api
{
    int klog(lua_state_t* L);
    int line(lua_state_t* L);
    void initialize();
}