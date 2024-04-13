cma_ib_new_udp_id(const struct rdma_cm_id *listen_id,
		  const struct ib_cm_event *ib_event,
		  struct net_device *net_dev)
{
	const struct rdma_id_private *listen_id_priv;
	struct rdma_id_private *id_priv;
	struct rdma_cm_id *id;
	const sa_family_t ss_family = listen_id->route.addr.src_addr.ss_family;
	struct net *net = listen_id->route.addr.dev_addr.net;
	int ret;

	listen_id_priv = container_of(listen_id, struct rdma_id_private, id);
	id_priv = __rdma_create_id(net, listen_id->event_handler,
				   listen_id->context, listen_id->ps, IB_QPT_UD,
				   listen_id_priv);
	if (IS_ERR(id_priv))
		return NULL;

	id = &id_priv->id;
	if (cma_save_net_info((struct sockaddr *)&id->route.addr.src_addr,
			      (struct sockaddr *)&id->route.addr.dst_addr,
			      listen_id, ib_event, ss_family,
			      ib_event->param.sidr_req_rcvd.service_id))
		goto err;

	if (net_dev) {
		rdma_copy_src_l2_addr(&id->route.addr.dev_addr, net_dev);
	} else {
		if (!cma_any_addr(cma_src_addr(id_priv))) {
			ret = cma_translate_addr(cma_src_addr(id_priv),
						 &id->route.addr.dev_addr);
			if (ret)
				goto err;
		}
	}

	id_priv->state = RDMA_CM_CONNECT;
	return id_priv;
err:
	rdma_destroy_id(id);
	return NULL;
}