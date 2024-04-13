static int fastrpc_dmabuf_alloc(struct fastrpc_user *fl, char __user *argp)
{
	struct fastrpc_alloc_dma_buf bp;
	DEFINE_DMA_BUF_EXPORT_INFO(exp_info);
	struct fastrpc_buf *buf = NULL;
	int err;

	if (copy_from_user(&bp, argp, sizeof(bp)))
		return -EFAULT;

	err = fastrpc_buf_alloc(fl, fl->sctx->dev, bp.size, &buf);
	if (err)
		return err;
	exp_info.ops = &fastrpc_dma_buf_ops;
	exp_info.size = bp.size;
	exp_info.flags = O_RDWR;
	exp_info.priv = buf;
	buf->dmabuf = dma_buf_export(&exp_info);
	if (IS_ERR(buf->dmabuf)) {
		err = PTR_ERR(buf->dmabuf);
		fastrpc_buf_free(buf);
		return err;
	}

	bp.fd = dma_buf_fd(buf->dmabuf, O_ACCMODE);
	if (bp.fd < 0) {
		dma_buf_put(buf->dmabuf);
		return -EINVAL;
	}

	if (copy_to_user(argp, &bp, sizeof(bp))) {
		dma_buf_put(buf->dmabuf);
		return -EFAULT;
	}

	return 0;
}