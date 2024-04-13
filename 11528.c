static int fastrpc_rpmsg_probe(struct rpmsg_device *rpdev)
{
	struct device *rdev = &rpdev->dev;
	struct fastrpc_channel_ctx *data;
	int i, err, domain_id = -1;
	const char *domain;

	err = of_property_read_string(rdev->of_node, "label", &domain);
	if (err) {
		dev_info(rdev, "FastRPC Domain not specified in DT\n");
		return err;
	}

	for (i = 0; i <= CDSP_DOMAIN_ID; i++) {
		if (!strcmp(domains[i], domain)) {
			domain_id = i;
			break;
		}
	}

	if (domain_id < 0) {
		dev_info(rdev, "FastRPC Invalid Domain ID %d\n", domain_id);
		return -EINVAL;
	}

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->miscdev.minor = MISC_DYNAMIC_MINOR;
	data->miscdev.name = kasprintf(GFP_KERNEL, "fastrpc-%s",
				domains[domain_id]);
	data->miscdev.fops = &fastrpc_fops;
	err = misc_register(&data->miscdev);
	if (err)
		return err;

	kref_init(&data->refcount);

	dev_set_drvdata(&rpdev->dev, data);
	dma_set_mask_and_coherent(rdev, DMA_BIT_MASK(32));
	INIT_LIST_HEAD(&data->users);
	spin_lock_init(&data->lock);
	idr_init(&data->ctx_idr);
	data->domain_id = domain_id;
	data->rpdev = rpdev;

	return of_platform_populate(rdev->of_node, NULL, NULL, rdev);
}