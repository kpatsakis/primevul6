EmitMousePositionSeparator(TScreen *screen, Char line[], unsigned count)
{
    switch (screen->extend_coords) {
    case SET_SGR_EXT_MODE_MOUSE:
    case SET_URXVT_EXT_MODE_MOUSE:
    case SET_PIXEL_POSITION_MOUSE:
	line[count++] = ';';
	break;
    }
    return count;
}