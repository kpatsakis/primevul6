line_capability(const char *name)
{
    bool result = FALSE;
    static const char *table[] =
    {
	"csr",			/* change_scroll_region          */
	"clear",		/* clear_screen                  */
	"ed",			/* clr_eos                       */
	"cwin",			/* create_window                 */
	"cup",			/* cursor_address                */
	"cud1",			/* cursor_down                   */
	"home",			/* cursor_home                   */
	"mrcup",		/* cursor_mem_address            */
	"ll",			/* cursor_to_ll                  */
	"cuu1",			/* cursor_up                     */
	"dl1",			/* delete_line                   */
	"hd",			/* down_half_line                */
	"flash",		/* flash_screen                  */
	"ff",			/* form_feed                     */
	"il1",			/* insert_line                   */
	"nel",			/* newline                       */
	"dl",			/* parm_delete_line              */
	"cud",			/* parm_down_cursor              */
	"indn",			/* parm_index                    */
	"il",			/* parm_insert_line              */
	"rin",			/* parm_rindex                   */
	"cuu",			/* parm_up_cursor                */
	"mc0",			/* print_screen                  */
	"vpa",			/* row_address                   */
	"ind",			/* scroll_forward                */
	"ri",			/* scroll_reverse                */
	"hu",			/* up_half_line                  */
    };
    size_t n;
    for (n = 0; n < SIZEOF(table); ++n) {
	if (!strcmp(name, table[n])) {
	    result = TRUE;
	    break;
	}
    }
    return result;
}