static int get_vdev_port_node_info(struct mdesc_handle *md, u64 node,
				   union md_node_info *node_info)
{
	const u64 *parent_cfg_hdlp;
	const char *name;
	const u64 *idp;

	/*
	 * Virtual device nodes are distinguished by:
	 * 1. "id" property
	 * 2. "name" property
	 * 3. parent node "cfg-handle" property
	 */
	idp = mdesc_get_property(md, node, "id", NULL);
	name = mdesc_get_property(md, node, "name", NULL);
	parent_cfg_hdlp = parent_cfg_handle(md, node);

	if (!idp || !name || !parent_cfg_hdlp)
		return -1;

	node_info->vdev_port.id = *idp;
	node_info->vdev_port.name = kstrdup_const(name, GFP_KERNEL);
	node_info->vdev_port.parent_cfg_hdl = *parent_cfg_hdlp;

	return 0;
}