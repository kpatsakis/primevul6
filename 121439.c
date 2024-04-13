eventRow(TScreen *screen, XEvent *event)	/* must be XButtonEvent */
{
    return (event->xbutton.y - screen->border) / FontHeight(screen);
}