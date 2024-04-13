static int fastrpc_buf_alloc(struct fastrpc_user *fl, struct device *dev,
			     u64 size, struct fastrpc_buf **obuf)
{
	struct fastrpc_buf *buf;

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	INIT_LIST_HEAD(&buf->attachments);
	mutex_init(&buf->lock);

	buf->fl = fl;
	buf->virt = NULL;
	buf->phys = 0;
	buf->size = size;
	buf->dev = dev;

	buf->virt = dma_alloc_coherent(dev, buf->size, (dma_addr_t *)&buf->phys,
				       GFP_KERNEL);
	if (!buf->virt) {
		mutex_destroy(&buf->lock);
		kfree(buf);
		return -ENOMEM;
	}

	if (fl->sctx && fl->sctx->sid)
		buf->phys += ((u64)fl->sctx->sid << 32);

	*obuf = buf;

	return 0;
}