qf_jump_first(qf_info_T *qi, int_u save_qfid, int forceit)
{
    if (qf_restore_list(qi, save_qfid) == FAIL)
	return;

    // Autocommands might have cleared the list, check for that.
    if (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}