wc_set_font_name(opttype, fontname)
int opttype;
char *fontname;
{
	char **fn = (char **)0;
	if (!fontname) return;
	switch(opttype) {
	    case MAP_OPTION:
			fn = &iflags.wc_font_map;
			break;
	    case MESSAGE_OPTION:
			fn = &iflags.wc_font_message;
			break;
	    case TEXT_OPTION:
			fn = &iflags.wc_font_text;
			break;
	    case MENU_OPTION:
			fn = &iflags.wc_font_menu;
			break;
	    case STATUS_OPTION:
			fn = &iflags.wc_font_status;
			break;
	    default:
			return;
	}
	if (fn) {
		if (*fn) free(*fn);
		*fn = (char *)alloc(strlen(fontname) + 1);
		Strcpy(*fn, fontname);
	}
	return;
}