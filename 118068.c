map_menu_cmd(ch)
    char ch;
{
    char *found = index(mapped_menu_cmds, ch);
    if (found) {
	int idx = (int)(found - mapped_menu_cmds);
	ch = mapped_menu_op[idx];
    }
    return ch;
}