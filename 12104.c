qf_cleanup_state(qfstate_T *pstate)
{
    if (pstate->fd != NULL)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    if (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, NULL, NULL);
}