add_menu_cmd_alias(from_ch, to_ch)
    char from_ch, to_ch;
{
    if (n_menu_mapped >= MAX_MENU_MAPPED_CMDS)
	pline("out of menu map space.");
    else {
	mapped_menu_cmds[n_menu_mapped] = from_ch;
	mapped_menu_op[n_menu_mapped] = to_ch;
	n_menu_mapped++;
	mapped_menu_cmds[n_menu_mapped] = 0;
	mapped_menu_op[n_menu_mapped] = 0;
    }
}