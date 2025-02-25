static long vhost_net_set_backend(struct vhost_net *n, unsigned index, int fd)
{
	struct socket *sock, *oldsock;
	struct vhost_virtqueue *vq;
	struct vhost_ubuf_ref *ubufs, *oldubufs = NULL;
	int r;

	mutex_lock(&n->dev.mutex);
	r = vhost_dev_check_owner(&n->dev);
	if (r)
		goto err;

	if (index >= VHOST_NET_VQ_MAX) {
		r = -ENOBUFS;
		goto err;
	}
	vq = n->vqs + index;
	mutex_lock(&vq->mutex);

	/* Verify that ring has been setup correctly. */
	if (!vhost_vq_access_ok(vq)) {
		r = -EFAULT;
		goto err_vq;
	}
	sock = get_socket(fd);
	if (IS_ERR(sock)) {
		r = PTR_ERR(sock);
		goto err_vq;
	}

	/* start polling new socket */
	oldsock = rcu_dereference_protected(vq->private_data,
					    lockdep_is_held(&vq->mutex));
	if (sock != oldsock) {
		ubufs = vhost_ubuf_alloc(vq, sock && vhost_sock_zcopy(sock));
		if (IS_ERR(ubufs)) {
			r = PTR_ERR(ubufs);
			goto err_ubufs;
		}
		oldubufs = vq->ubufs;
		vq->ubufs = ubufs;
		vhost_net_disable_vq(n, vq);
		rcu_assign_pointer(vq->private_data, sock);
		vhost_net_enable_vq(n, vq);

		r = vhost_init_used(vq);
		if (r)
			goto err_vq;

		n->tx_packets = 0;
		n->tx_zcopy_err = 0;
	}

	mutex_unlock(&vq->mutex);

	if (oldubufs) {
		vhost_ubuf_put_and_wait(oldubufs);
		mutex_lock(&vq->mutex);
		vhost_zerocopy_signal_used(n, vq);
		mutex_unlock(&vq->mutex);
	}

	if (oldsock) {
		vhost_net_flush_vq(n, index);
		fput(oldsock->file);
	}

	mutex_unlock(&n->dev.mutex);
	return 0;

err_ubufs:
	fput(sock->file);
err_vq:
	mutex_unlock(&vq->mutex);
err:
	mutex_unlock(&n->dev.mutex);
	return r;
}