handle_sysmouse(int sig GCC_UNUSED)
{
    sysmouse_server(CURRENT_SCREEN);
}