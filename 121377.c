ReadLineMovePoint(TScreen *screen, int col, int ldelta)
{
    Char line[6];
    unsigned count = 0;

    col += ldelta * MaxCols(screen) - screen->cur_col;
    if (col == 0)
	return 0;
    if (screen->control_eight_bits) {
	line[count++] = ANSI_CSI;
    } else {
	line[count++] = ANSI_ESC;
	line[count++] = '[';	/* XXX maybe sometimes O is better? */
    }
    line[count] = CharOf(col > 0 ? 'C' : 'D');
    if (col < 0)
	col = -col;
    while (col--)
	v_write(screen->respond, line, 3);
    return 1;
}