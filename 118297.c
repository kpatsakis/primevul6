cma_iboe_set_path_rec_l2_fields(struct rdma_id_private *id_priv)
{
	struct rdma_route *route = &id_priv->id.route;
	enum ib_gid_type gid_type = IB_GID_TYPE_ROCE;
	struct rdma_addr *addr = &route->addr;
	unsigned long supported_gids;
	struct net_device *ndev;

	if (!addr->dev_addr.bound_dev_if)
		return NULL;

	ndev = dev_get_by_index(addr->dev_addr.net,
				addr->dev_addr.bound_dev_if);
	if (!ndev)
		return NULL;

	supported_gids = roce_gid_type_mask_support(id_priv->id.device,
						    id_priv->id.port_num);
	gid_type = cma_route_gid_type(addr->dev_addr.network,
				      supported_gids,
				      id_priv->gid_type);
	/* Use the hint from IP Stack to select GID Type */
	if (gid_type < ib_network_to_gid_type(addr->dev_addr.network))
		gid_type = ib_network_to_gid_type(addr->dev_addr.network);
	route->path_rec->rec_type = sa_conv_gid_to_pathrec_type(gid_type);

	route->path_rec->roce.route_resolved = true;
	sa_path_set_dmac(route->path_rec, addr->dev_addr.dst_dev_addr);
	return ndev;
}