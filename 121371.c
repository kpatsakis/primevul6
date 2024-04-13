_WriteKey(TScreen *screen, const Char *in)
{
    Char line[16];
    unsigned count = 0;
    size_t length = strlen((const char *) in);

    if (screen->control_eight_bits) {
	line[count++] = ANSI_CSI;
    } else {
	line[count++] = ANSI_ESC;
	line[count++] = '[';
    }
    while (length--)
	line[count++] = *in++;
    line[count++] = '~';
    tty_vwrite(screen->respond, line, count);
}