load_gpm_library(SCREEN *sp)
{
    sp->_mouse_gpm_found = FALSE;
    if ((sp->_dlopen_gpm = dlopen(LIBGPM_SONAME, my_RTLD)) != 0) {
#if (defined(__GNUC__) && (__GNUC__ >= 5)) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
	if (GET_DLSYM(gpm_fd) == 0 ||
	    GET_DLSYM(Gpm_Open) == 0 ||
	    GET_DLSYM(Gpm_Close) == 0 ||
	    GET_DLSYM(Gpm_GetEvent) == 0) {
#if (defined(__GNUC__) && (__GNUC__ >= 5)) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
	    T(("GPM initialization failed: %s", dlerror()));
	    unload_gpm_library(sp);
	} else {
	    sp->_mouse_gpm_found = TRUE;
	    sp->_mouse_gpm_loaded = TRUE;
	}
    }
}