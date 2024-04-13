static int fastrpc_release_current_dsp_process(struct fastrpc_user *fl)
{
	struct fastrpc_invoke_args args[1];
	int tgid = 0;
	u32 sc;

	tgid = fl->tgid;
	args[0].ptr = (u64)(uintptr_t) &tgid;
	args[0].length = sizeof(tgid);
	args[0].fd = -1;
	args[0].reserved = 0;
	sc = FASTRPC_SCALARS(FASTRPC_RMID_INIT_RELEASE, 1, 0);

	return fastrpc_internal_invoke(fl, true, FASTRPC_INIT_HANDLE,
				       sc, &args[0]);
}