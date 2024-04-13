eventColBetween(TScreen *screen, XEvent *event)		/* must be XButtonEvent */
{
    /* Correct by half a width - we are acting on a boundary, not on a cell. */
    return ((event->xbutton.x - OriginX(screen) + (FontWidth(screen) - 1) / 2)
	    / FontWidth(screen));
}