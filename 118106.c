choose_classes_menu(prompt, category, way, class_list, class_select)
const char *prompt;
int category;
boolean way;
char *class_list;
char *class_select;
{
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    char buf[BUFSZ];
    int i, n;
    int ret;
    int next_accelerator, accelerator;

    if (class_list == (char *)0 || class_select == (char *)0) return 0;
    accelerator = 0;
    next_accelerator = 'a';
    any = zeroany;
    win = create_nhwindow(NHW_MENU);
    start_menu(win);
    while (*class_list) {
	const char *text;
	boolean selected;

	text = (char *)0;
	selected = FALSE;
	switch (category) {
		case 0:
			text = def_monsyms[def_char_to_monclass(*class_list)].explain;
			accelerator = *class_list;
			Sprintf(buf, "%s", text);
			break;
		case 1:
			text = def_oc_syms[def_char_to_objclass(*class_list)].explain;
			accelerator = next_accelerator;
			Sprintf(buf, "%c  %s", *class_list, text);
			break;
		default:
			impossible("choose_classes_menu: invalid category %d",
					category);
	}
	if (way && *class_select) {	/* Selections there already */
		if (index(class_select, *class_list)) {
			selected = TRUE;
		}
	}
	any.a_int = *class_list;
	add_menu(win, NO_GLYPH, &any, accelerator,
		  category ? *class_list : 0,
		  ATR_NONE, buf, selected);
	++class_list;
	if (category > 0) {
		++next_accelerator;
		if (next_accelerator == ('z' + 1)) next_accelerator = 'A';
		if (next_accelerator == ('Z' + 1)) break;
	}
    }
    end_menu(win, prompt);
    n = select_menu(win, way ? PICK_ANY : PICK_ONE, &pick_list);
    destroy_nhwindow(win);
    if (n > 0) {
	for (i = 0; i < n; ++i)
	    *class_select++ = (char)pick_list[i].item.a_int;
	free((genericptr_t)pick_list);
	ret = n;
    } else if (n == -1) {
	class_select = eos(class_select);
	ret = -1;
    } else
	ret = 0;
    *class_select = '\0';
    return ret;
}