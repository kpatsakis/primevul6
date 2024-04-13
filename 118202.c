cma_ib_new_conn_id(const struct rdma_cm_id *listen_id,
		   const struct ib_cm_event *ib_event,
		   struct net_device *net_dev)
{
	struct rdma_id_private *listen_id_priv;
	struct rdma_id_private *id_priv;
	struct rdma_cm_id *id;
	struct rdma_route *rt;
	const sa_family_t ss_family = listen_id->route.addr.src_addr.ss_family;
	struct sa_path_rec *path = ib_event->param.req_rcvd.primary_path;
	const __be64 service_id =
		ib_event->param.req_rcvd.primary_path->service_id;
	int ret;

	listen_id_priv = container_of(listen_id, struct rdma_id_private, id);
	id_priv = __rdma_create_id(listen_id->route.addr.dev_addr.net,
				   listen_id->event_handler, listen_id->context,
				   listen_id->ps,
				   ib_event->param.req_rcvd.qp_type,
				   listen_id_priv);
	if (IS_ERR(id_priv))
		return NULL;

	id = &id_priv->id;
	if (cma_save_net_info((struct sockaddr *)&id->route.addr.src_addr,
			      (struct sockaddr *)&id->route.addr.dst_addr,
			      listen_id, ib_event, ss_family, service_id))
		goto err;

	rt = &id->route;
	rt->num_paths = ib_event->param.req_rcvd.alternate_path ? 2 : 1;
	rt->path_rec = kmalloc_array(rt->num_paths, sizeof(*rt->path_rec),
				     GFP_KERNEL);
	if (!rt->path_rec)
		goto err;

	rt->path_rec[0] = *path;
	if (rt->num_paths == 2)
		rt->path_rec[1] = *ib_event->param.req_rcvd.alternate_path;

	if (net_dev) {
		rdma_copy_src_l2_addr(&rt->addr.dev_addr, net_dev);
	} else {
		if (!cma_protocol_roce(listen_id) &&
		    cma_any_addr(cma_src_addr(id_priv))) {
			rt->addr.dev_addr.dev_type = ARPHRD_INFINIBAND;
			rdma_addr_set_sgid(&rt->addr.dev_addr, &rt->path_rec[0].sgid);
			ib_addr_set_pkey(&rt->addr.dev_addr, be16_to_cpu(rt->path_rec[0].pkey));
		} else if (!cma_any_addr(cma_src_addr(id_priv))) {
			ret = cma_translate_addr(cma_src_addr(id_priv), &rt->addr.dev_addr);
			if (ret)
				goto err;
		}
	}
	rdma_addr_set_dgid(&rt->addr.dev_addr, &rt->path_rec[0].dgid);

	id_priv->state = RDMA_CM_CONNECT;
	return id_priv;

err:
	rdma_destroy_id(id);
	return NULL;
}