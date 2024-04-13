static int rpmsg_probe(struct virtio_device *vdev)
{
	vq_callback_t *vq_cbs[] = { rpmsg_recv_done, rpmsg_xmit_done };
	static const char * const names[] = { "input", "output" };
	struct virtqueue *vqs[2];
	struct virtproc_info *vrp;
	struct virtio_rpmsg_channel *vch = NULL;
	struct rpmsg_device *rpdev_ns, *rpdev_ctrl;
	void *bufs_va;
	int err = 0, i;
	size_t total_buf_space;
	bool notify;

	vrp = kzalloc(sizeof(*vrp), GFP_KERNEL);
	if (!vrp)
		return -ENOMEM;

	vrp->vdev = vdev;

	idr_init(&vrp->endpoints);
	mutex_init(&vrp->endpoints_lock);
	mutex_init(&vrp->tx_lock);
	init_waitqueue_head(&vrp->sendq);

	/* We expect two virtqueues, rx and tx (and in this order) */
	err = virtio_find_vqs(vdev, 2, vqs, vq_cbs, names, NULL);
	if (err)
		goto free_vrp;

	vrp->rvq = vqs[0];
	vrp->svq = vqs[1];

	/* we expect symmetric tx/rx vrings */
	WARN_ON(virtqueue_get_vring_size(vrp->rvq) !=
		virtqueue_get_vring_size(vrp->svq));

	/* we need less buffers if vrings are small */
	if (virtqueue_get_vring_size(vrp->rvq) < MAX_RPMSG_NUM_BUFS / 2)
		vrp->num_bufs = virtqueue_get_vring_size(vrp->rvq) * 2;
	else
		vrp->num_bufs = MAX_RPMSG_NUM_BUFS;

	vrp->buf_size = MAX_RPMSG_BUF_SIZE;

	total_buf_space = vrp->num_bufs * vrp->buf_size;

	/* allocate coherent memory for the buffers */
	bufs_va = dma_alloc_coherent(vdev->dev.parent,
				     total_buf_space, &vrp->bufs_dma,
				     GFP_KERNEL);
	if (!bufs_va) {
		err = -ENOMEM;
		goto vqs_del;
	}

	dev_dbg(&vdev->dev, "buffers: va %pK, dma %pad\n",
		bufs_va, &vrp->bufs_dma);

	/* half of the buffers is dedicated for RX */
	vrp->rbufs = bufs_va;

	/* and half is dedicated for TX */
	vrp->sbufs = bufs_va + total_buf_space / 2;

	/* set up the receive buffers */
	for (i = 0; i < vrp->num_bufs / 2; i++) {
		struct scatterlist sg;
		void *cpu_addr = vrp->rbufs + i * vrp->buf_size;

		rpmsg_sg_init(&sg, cpu_addr, vrp->buf_size);

		err = virtqueue_add_inbuf(vrp->rvq, &sg, 1, cpu_addr,
					  GFP_KERNEL);
		WARN_ON(err); /* sanity check; this can't really happen */
	}

	/* suppress "tx-complete" interrupts */
	virtqueue_disable_cb(vrp->svq);

	vdev->priv = vrp;

	rpdev_ctrl = rpmsg_virtio_add_ctrl_dev(vdev);
	if (IS_ERR(rpdev_ctrl)) {
		err = PTR_ERR(rpdev_ctrl);
		goto free_coherent;
	}

	/* if supported by the remote processor, enable the name service */
	if (virtio_has_feature(vdev, VIRTIO_RPMSG_F_NS)) {
		vch = kzalloc(sizeof(*vch), GFP_KERNEL);
		if (!vch) {
			err = -ENOMEM;
			goto free_ctrldev;
		}

		/* Link the channel to our vrp */
		vch->vrp = vrp;

		/* Assign public information to the rpmsg_device */
		rpdev_ns = &vch->rpdev;
		rpdev_ns->ops = &virtio_rpmsg_ops;
		rpdev_ns->little_endian = virtio_is_little_endian(vrp->vdev);

		rpdev_ns->dev.parent = &vrp->vdev->dev;
		rpdev_ns->dev.release = virtio_rpmsg_release_device;

		err = rpmsg_ns_register_device(rpdev_ns);
		if (err)
			goto free_vch;
	}

	/*
	 * Prepare to kick but don't notify yet - we can't do this before
	 * device is ready.
	 */
	notify = virtqueue_kick_prepare(vrp->rvq);

	/* From this point on, we can notify and get callbacks. */
	virtio_device_ready(vdev);

	/* tell the remote processor it can start sending messages */
	/*
	 * this might be concurrent with callbacks, but we are only
	 * doing notify, not a full kick here, so that's ok.
	 */
	if (notify)
		virtqueue_notify(vrp->rvq);

	dev_info(&vdev->dev, "rpmsg host is online\n");

	return 0;

free_vch:
	kfree(vch);
free_ctrldev:
	rpmsg_virtio_del_ctrl_dev(rpdev_ctrl);
free_coherent:
	dma_free_coherent(vdev->dev.parent, total_buf_space,
			  bufs_va, vrp->bufs_dma);
vqs_del:
	vdev->config->del_vqs(vrp->vdev);
free_vrp:
	kfree(vrp);
	return err;
}