static int fastrpc_get_args(u32 kernel, struct fastrpc_invoke_ctx *ctx)
{
	struct device *dev = ctx->fl->sctx->dev;
	struct fastrpc_remote_arg *rpra;
	struct fastrpc_invoke_buf *list;
	struct fastrpc_phy_page *pages;
	int inbufs, i, oix, err = 0;
	u64 len, rlen, pkt_size;
	u64 pg_start, pg_end;
	uintptr_t args;
	int metalen;

	inbufs = REMOTE_SCALARS_INBUFS(ctx->sc);
	metalen = fastrpc_get_meta_size(ctx);
	pkt_size = fastrpc_get_payload_size(ctx, metalen);

	err = fastrpc_create_maps(ctx);
	if (err)
		return err;

	ctx->msg_sz = pkt_size;

	err = fastrpc_buf_alloc(ctx->fl, dev, pkt_size, &ctx->buf);
	if (err)
		return err;

	rpra = ctx->buf->virt;
	list = ctx->buf->virt + ctx->nscalars * sizeof(*rpra);
	pages = ctx->buf->virt + ctx->nscalars * (sizeof(*list) +
		sizeof(*rpra));
	args = (uintptr_t)ctx->buf->virt + metalen;
	rlen = pkt_size - metalen;
	ctx->rpra = rpra;

	for (oix = 0; oix < ctx->nbufs; ++oix) {
		int mlen;

		i = ctx->olaps[oix].raix;
		len = ctx->args[i].length;

		rpra[i].pv = 0;
		rpra[i].len = len;
		list[i].num = len ? 1 : 0;
		list[i].pgidx = i;

		if (!len)
			continue;

		if (ctx->maps[i]) {
			struct vm_area_struct *vma = NULL;

			rpra[i].pv = (u64) ctx->args[i].ptr;
			pages[i].addr = ctx->maps[i]->phys;

			vma = find_vma(current->mm, ctx->args[i].ptr);
			if (vma)
				pages[i].addr += ctx->args[i].ptr -
						 vma->vm_start;

			pg_start = (ctx->args[i].ptr & PAGE_MASK) >> PAGE_SHIFT;
			pg_end = ((ctx->args[i].ptr + len - 1) & PAGE_MASK) >>
				  PAGE_SHIFT;
			pages[i].size = (pg_end - pg_start + 1) * PAGE_SIZE;

		} else {

			if (ctx->olaps[oix].offset == 0) {
				rlen -= ALIGN(args, FASTRPC_ALIGN) - args;
				args = ALIGN(args, FASTRPC_ALIGN);
			}

			mlen = ctx->olaps[oix].mend - ctx->olaps[oix].mstart;

			if (rlen < mlen)
				goto bail;

			rpra[i].pv = args - ctx->olaps[oix].offset;
			pages[i].addr = ctx->buf->phys -
					ctx->olaps[oix].offset +
					(pkt_size - rlen);
			pages[i].addr = pages[i].addr &	PAGE_MASK;

			pg_start = (args & PAGE_MASK) >> PAGE_SHIFT;
			pg_end = ((args + len - 1) & PAGE_MASK) >> PAGE_SHIFT;
			pages[i].size = (pg_end - pg_start + 1) * PAGE_SIZE;
			args = args + mlen;
			rlen -= mlen;
		}

		if (i < inbufs && !ctx->maps[i]) {
			void *dst = (void *)(uintptr_t)rpra[i].pv;
			void *src = (void *)(uintptr_t)ctx->args[i].ptr;

			if (!kernel) {
				if (copy_from_user(dst, (void __user *)src,
						   len)) {
					err = -EFAULT;
					goto bail;
				}
			} else {
				memcpy(dst, src, len);
			}
		}
	}

	for (i = ctx->nbufs; i < ctx->nscalars; ++i) {
		rpra[i].pv = (u64) ctx->args[i].ptr;
		rpra[i].len = ctx->args[i].length;
		list[i].num = ctx->args[i].length ? 1 : 0;
		list[i].pgidx = i;
		pages[i].addr = ctx->maps[i]->phys;
		pages[i].size = ctx->maps[i]->size;
	}

bail:
	if (err)
		dev_err(dev, "Error: get invoke args failed:%d\n", err);

	return err;
}