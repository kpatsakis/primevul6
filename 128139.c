vim_dialog_yesnoallcancel(
    int		type,
    char_u	*title,
    char_u	*message,
    int		dflt)
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)"Question" : title,
		message,
		(char_u *)_("&Yes\n&No\nSave &All\n&Discard All\n&Cancel"),
							   dflt, NULL, FALSE))
    {
	case 1: return VIM_YES;
	case 2: return VIM_NO;
	case 3: return VIM_ALL;
	case 4: return VIM_DISCARDALL;
    }
    return VIM_CANCEL;
}