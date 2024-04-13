sysmouse_server(SCREEN *sp)
{
    struct mouse_info the_mouse;
    MEVENT *work;

    the_mouse.operation = MOUSE_GETINFO;
    if (sp != 0
	&& sp->_mouse_fd >= 0
	&& sp->_sysmouse_tail < FIFO_SIZE
	&& ioctl(sp->_mouse_fd, CONS_MOUSECTL, &the_mouse) != -1) {

	if (sp->_sysmouse_head > sp->_sysmouse_tail) {
	    sp->_sysmouse_tail = 0;
	    sp->_sysmouse_head = 0;
	}
	work = &(sp->_sysmouse_fifo[sp->_sysmouse_tail]);
	memset(work, 0, sizeof(*work));
	work->id = NORMAL_EVENT;	/* there's only one mouse... */

	sp->_sysmouse_old_buttons = sp->_sysmouse_new_buttons;
	sp->_sysmouse_new_buttons = the_mouse.u.data.buttons & 0x7;

	if (sp->_sysmouse_new_buttons) {
	    if (sp->_sysmouse_new_buttons & 1)
		work->bstate |= BUTTON1_PRESSED;
	    if (sp->_sysmouse_new_buttons & 2)
		work->bstate |= BUTTON2_PRESSED;
	    if (sp->_sysmouse_new_buttons & 4)
		work->bstate |= BUTTON3_PRESSED;
	} else {
	    if (sp->_sysmouse_old_buttons & 1)
		work->bstate |= BUTTON1_RELEASED;
	    if (sp->_sysmouse_old_buttons & 2)
		work->bstate |= BUTTON2_RELEASED;
	    if (sp->_sysmouse_old_buttons & 4)
		work->bstate |= BUTTON3_RELEASED;
	}

	/* for cosmetic bug in syscons.c on FreeBSD 3.[34] */
	the_mouse.operation = MOUSE_HIDE;
	ioctl(sp->_mouse_fd, CONS_MOUSECTL, &the_mouse);
	the_mouse.operation = MOUSE_SHOW;
	ioctl(sp->_mouse_fd, CONS_MOUSECTL, &the_mouse);

	/*
	 * We're only interested if the button is pressed or released.
	 * FIXME: implement continuous event-tracking.
	 */
	if (sp->_sysmouse_new_buttons != sp->_sysmouse_old_buttons) {
	    sp->_sysmouse_tail += 1;
	}
	work->x = the_mouse.u.data.x / sp->_sysmouse_char_width;
	work->y = the_mouse.u.data.y / sp->_sysmouse_char_height;
    }
}