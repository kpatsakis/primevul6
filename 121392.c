ReadLineDelete(TScreen *screen, CELL *cell1, CELL *cell2)
{
    int del;

    del = (cell2->col - cell1->col) + ((cell2->row - cell1->row) * MaxCols(screen));
    if (del <= 0)		/* Just in case... */
	return 0;
    while (del--)
	v_write(screen->respond, (const Char *) "\177", 1);
    return 1;
}