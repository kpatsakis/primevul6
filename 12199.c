ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    if (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	return;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_expr(eap->arg, eap);
    if (tv != NULL)
    {
	(void)cexpr_core(eap, tv);
	free_tv(tv);
    }
}