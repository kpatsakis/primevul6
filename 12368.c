handle_tabmenu(void)
{
    switch (current_tabmenu)
    {
	case TABLINE_MENU_CLOSE:
	    if (current_tab == 0)
		do_cmdline_cmd((char_u *)"tabclose");
	    else
	    {
		vim_snprintf((char *)IObuff, IOSIZE, "tabclose %d",
								 current_tab);
		do_cmdline_cmd(IObuff);
	    }
	    break;

	case TABLINE_MENU_NEW:
	    if (current_tab == 0)
		do_cmdline_cmd((char_u *)"$tabnew");
	    else
	    {
		vim_snprintf((char *)IObuff, IOSIZE, "%dtabnew",
							     current_tab - 1);
		do_cmdline_cmd(IObuff);
	    }
	    break;

	case TABLINE_MENU_OPEN:
	    if (current_tab == 0)
		do_cmdline_cmd((char_u *)"browse $tabnew");
	    else
	    {
		vim_snprintf((char *)IObuff, IOSIZE, "browse %dtabnew",
							     current_tab - 1);
		do_cmdline_cmd(IObuff);
	    }
	    break;
    }
}