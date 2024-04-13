static int fastrpc_init_attach(struct fastrpc_user *fl)
{
	struct fastrpc_invoke_args args[1];
	int tgid = fl->tgid;
	u32 sc;

	args[0].ptr = (u64)(uintptr_t) &tgid;
	args[0].length = sizeof(tgid);
	args[0].fd = -1;
	args[0].reserved = 0;
	sc = FASTRPC_SCALARS(FASTRPC_RMID_INIT_ATTACH, 1, 0);
	fl->pd = 0;

	return fastrpc_internal_invoke(fl, true, FASTRPC_INIT_HANDLE,
				       sc, &args[0]);
}