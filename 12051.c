qf_entry_after_pos(qfline_T *qfp, pos_T *pos, int linewise)
{
    if (linewise)
	return qfp->qf_lnum > pos->lnum;
    else
	return (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}