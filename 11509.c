static int fastrpc_init_create_process(struct fastrpc_user *fl,
					char __user *argp)
{
	struct fastrpc_init_create init;
	struct fastrpc_invoke_args *args;
	struct fastrpc_phy_page pages[1];
	struct fastrpc_map *map = NULL;
	struct fastrpc_buf *imem = NULL;
	int memlen;
	int err;
	struct {
		int pgid;
		u32 namelen;
		u32 filelen;
		u32 pageslen;
		u32 attrs;
		u32 siglen;
	} inbuf;
	u32 sc;

	args = kcalloc(FASTRPC_CREATE_PROCESS_NARGS, sizeof(*args), GFP_KERNEL);
	if (!args)
		return -ENOMEM;

	if (copy_from_user(&init, argp, sizeof(init))) {
		err = -EFAULT;
		goto err;
	}

	if (init.filelen > INIT_FILELEN_MAX) {
		err = -EINVAL;
		goto err;
	}

	inbuf.pgid = fl->tgid;
	inbuf.namelen = strlen(current->comm) + 1;
	inbuf.filelen = init.filelen;
	inbuf.pageslen = 1;
	inbuf.attrs = init.attrs;
	inbuf.siglen = init.siglen;
	fl->pd = 1;

	if (init.filelen && init.filefd) {
		err = fastrpc_map_create(fl, init.filefd, init.filelen, &map);
		if (err)
			goto err;
	}

	memlen = ALIGN(max(INIT_FILELEN_MAX, (int)init.filelen * 4),
		       1024 * 1024);
	err = fastrpc_buf_alloc(fl, fl->sctx->dev, memlen,
				&imem);
	if (err)
		goto err_alloc;

	fl->init_mem = imem;
	args[0].ptr = (u64)(uintptr_t)&inbuf;
	args[0].length = sizeof(inbuf);
	args[0].fd = -1;

	args[1].ptr = (u64)(uintptr_t)current->comm;
	args[1].length = inbuf.namelen;
	args[1].fd = -1;

	args[2].ptr = (u64) init.file;
	args[2].length = inbuf.filelen;
	args[2].fd = init.filefd;

	pages[0].addr = imem->phys;
	pages[0].size = imem->size;

	args[3].ptr = (u64)(uintptr_t) pages;
	args[3].length = 1 * sizeof(*pages);
	args[3].fd = -1;

	args[4].ptr = (u64)(uintptr_t)&inbuf.attrs;
	args[4].length = sizeof(inbuf.attrs);
	args[4].fd = -1;

	args[5].ptr = (u64)(uintptr_t) &inbuf.siglen;
	args[5].length = sizeof(inbuf.siglen);
	args[5].fd = -1;

	sc = FASTRPC_SCALARS(FASTRPC_RMID_INIT_CREATE, 4, 0);
	if (init.attrs)
		sc = FASTRPC_SCALARS(FASTRPC_RMID_INIT_CREATE_ATTR, 6, 0);

	err = fastrpc_internal_invoke(fl, true, FASTRPC_INIT_HANDLE,
				      sc, args);
	if (err)
		goto err_invoke;

	kfree(args);

	return 0;

err_invoke:
	fl->init_mem = NULL;
	fastrpc_buf_free(imem);
err_alloc:
	if (map) {
		spin_lock(&fl->lock);
		list_del(&map->node);
		spin_unlock(&fl->lock);
		fastrpc_map_put(map);
	}
err:
	kfree(args);

	return err;
}