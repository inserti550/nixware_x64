void hooks::handles::level_init(void* ecx, const char* map_name)
{
	globals::lua_initialized = false;

    return originals::level_init(ecx, map_name);
}