MouseLimit(TScreen *screen)
{
    int mouse_limit;

    switch (screen->extend_coords) {
    default:
	mouse_limit = MOUSE_LIMIT;
	break;
    case SET_EXT_MODE_MOUSE:
	mouse_limit = EXT_MOUSE_LIMIT;
	break;
    case SET_SGR_EXT_MODE_MOUSE:
    case SET_URXVT_EXT_MODE_MOUSE:
    case SET_PIXEL_POSITION_MOUSE:
	mouse_limit = -1;
	break;
    }
    return mouse_limit;
}