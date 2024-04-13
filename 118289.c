cma_ib_id_from_event(struct ib_cm_id *cm_id,
		     const struct ib_cm_event *ib_event,
		     struct cma_req_info *req,
		     struct net_device **net_dev)
{
	struct rdma_bind_list *bind_list;
	struct rdma_id_private *id_priv;
	int err;

	err = cma_save_req_info(ib_event, req);
	if (err)
		return ERR_PTR(err);

	*net_dev = cma_get_net_dev(ib_event, req);
	if (IS_ERR(*net_dev)) {
		if (PTR_ERR(*net_dev) == -EAFNOSUPPORT) {
			/* Assuming the protocol is AF_IB */
			*net_dev = NULL;
		} else {
			return ERR_CAST(*net_dev);
		}
	}

	mutex_lock(&lock);
	/*
	 * Net namespace might be getting deleted while route lookup,
	 * cm_id lookup is in progress. Therefore, perform netdevice
	 * validation, cm_id lookup under rcu lock.
	 * RCU lock along with netdevice state check, synchronizes with
	 * netdevice migrating to different net namespace and also avoids
	 * case where net namespace doesn't get deleted while lookup is in
	 * progress.
	 * If the device state is not IFF_UP, its properties such as ifindex
	 * and nd_net cannot be trusted to remain valid without rcu lock.
	 * net/core/dev.c change_net_namespace() ensures to synchronize with
	 * ongoing operations on net device after device is closed using
	 * synchronize_net().
	 */
	rcu_read_lock();
	if (*net_dev) {
		/*
		 * If netdevice is down, it is likely that it is administratively
		 * down or it might be migrating to different namespace.
		 * In that case avoid further processing, as the net namespace
		 * or ifindex may change.
		 */
		if (((*net_dev)->flags & IFF_UP) == 0) {
			id_priv = ERR_PTR(-EHOSTUNREACH);
			goto err;
		}

		if (!validate_net_dev(*net_dev,
				 (struct sockaddr *)&req->listen_addr_storage,
				 (struct sockaddr *)&req->src_addr_storage)) {
			id_priv = ERR_PTR(-EHOSTUNREACH);
			goto err;
		}
	}

	bind_list = cma_ps_find(*net_dev ? dev_net(*net_dev) : &init_net,
				rdma_ps_from_service_id(req->service_id),
				cma_port_from_service_id(req->service_id));
	id_priv = cma_find_listener(bind_list, cm_id, ib_event, req, *net_dev);
err:
	rcu_read_unlock();
	mutex_unlock(&lock);
	if (IS_ERR(id_priv) && *net_dev) {
		dev_put(*net_dev);
		*net_dev = NULL;
	}
	return id_priv;
}