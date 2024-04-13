static void isdn_ppp_ccp_kickup(struct ippp_struct *is)
{
	isdn_ppp_fill_rq(NULL, 0, PPP_COMP, is->lp->ppp_slot);
}