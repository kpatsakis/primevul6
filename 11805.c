unload_gpm_library(SCREEN *sp)
{
    if (sp->_dlopen_gpm != 0) {
	T(("unload GPM library"));
	sp->_mouse_gpm_loaded = FALSE;
	sp->_mouse_fd = -1;
	dlclose(sp->_dlopen_gpm);
	sp->_dlopen_gpm = 0;
    }
}