static int fastrpc_cb_probe(struct platform_device *pdev)
{
	struct fastrpc_channel_ctx *cctx;
	struct fastrpc_session_ctx *sess;
	struct device *dev = &pdev->dev;
	int i, sessions = 0;
	unsigned long flags;
	int rc;

	cctx = dev_get_drvdata(dev->parent);
	if (!cctx)
		return -EINVAL;

	of_property_read_u32(dev->of_node, "qcom,nsessions", &sessions);

	spin_lock_irqsave(&cctx->lock, flags);
	sess = &cctx->session[cctx->sesscount];
	sess->used = false;
	sess->valid = true;
	sess->dev = dev;
	dev_set_drvdata(dev, sess);

	if (of_property_read_u32(dev->of_node, "reg", &sess->sid))
		dev_info(dev, "FastRPC Session ID not specified in DT\n");

	if (sessions > 0) {
		struct fastrpc_session_ctx *dup_sess;

		for (i = 1; i < sessions; i++) {
			if (cctx->sesscount++ >= FASTRPC_MAX_SESSIONS)
				break;
			dup_sess = &cctx->session[cctx->sesscount];
			memcpy(dup_sess, sess, sizeof(*dup_sess));
		}
	}
	cctx->sesscount++;
	spin_unlock_irqrestore(&cctx->lock, flags);
	rc = dma_set_mask(dev, DMA_BIT_MASK(32));
	if (rc) {
		dev_err(dev, "32-bit DMA enable failed\n");
		return rc;
	}

	return 0;
}