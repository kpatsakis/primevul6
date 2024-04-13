vim_dialog_yesnocancel(
    int		type,
    char_u	*title,
    char_u	*message,
    int		dflt)
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No\n&Cancel"), dflt, NULL, FALSE))
    {
	case 1: return VIM_YES;
	case 2: return VIM_NO;
    }
    return VIM_CANCEL;
}