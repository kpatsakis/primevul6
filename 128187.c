vim_dialog_yesno(
    int		type,
    char_u	*title,
    char_u	*message,
    int		dflt)
{
    if (do_dialog(type,
		title == NULL ? (char_u *)_("Question") : title,
		message,
		(char_u *)_("&Yes\n&No"), dflt, NULL, FALSE) == 1)
	return VIM_YES;
    return VIM_NO;
}