base64_flush(TScreen *screen)
{
    Char x;

    TRACE(("base64_flush count %d, pad %d (%d)\n",
	   screen->base64_count,
	   screen->base64_pad,
	   screen->base64_pad & 3));

    switch (screen->base64_count) {
    case 0:
	break;
    case 2:
	x = CharOf(base64_code[screen->base64_accu << 4]);
	tty_vwrite(screen->respond, &x, 1);
	break;
    case 4:
	x = CharOf(base64_code[screen->base64_accu << 2]);
	tty_vwrite(screen->respond, &x, 1);
	break;
    }
    if (screen->base64_pad & 3) {
	tty_vwrite(screen->respond,
		   (const Char *) "===",
		   (unsigned) (3 - (screen->base64_pad & 3)));
    }
    screen->base64_count = 0;
    screen->base64_accu = 0;
    screen->base64_pad = 0;
}