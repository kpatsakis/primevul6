static int rsi_init_usb_interface(struct rsi_hw *adapter,
				  struct usb_interface *pfunction)
{
	struct rsi_91x_usbdev *rsi_dev;
	int status;

	rsi_dev = kzalloc(sizeof(*rsi_dev), GFP_KERNEL);
	if (!rsi_dev)
		return -ENOMEM;

	adapter->rsi_dev = rsi_dev;
	rsi_dev->usbdev = interface_to_usbdev(pfunction);
	rsi_dev->priv = (void *)adapter;

	if (rsi_find_bulk_in_and_out_endpoints(pfunction, adapter)) {
		status = -EINVAL;
		goto fail_eps;
	}

	adapter->device = &pfunction->dev;
	usb_set_intfdata(pfunction, adapter);

	rsi_dev->tx_buffer = kmalloc(2048, GFP_KERNEL);
	if (!rsi_dev->tx_buffer) {
		status = -ENOMEM;
		goto fail_eps;
	}

	if (rsi_usb_init_rx(adapter)) {
		rsi_dbg(ERR_ZONE, "Failed to init RX handle\n");
		status = -ENOMEM;
		goto fail_rx;
	}

	rsi_dev->tx_blk_size = 252;
	adapter->block_size = rsi_dev->tx_blk_size;

	/* Initializing function callbacks */
	adapter->check_hw_queue_status = rsi_usb_check_queue_status;
	adapter->determine_event_timeout = rsi_usb_event_timeout;
	adapter->rsi_host_intf = RSI_HOST_INTF_USB;
	adapter->host_intf_ops = &usb_host_intf_ops;

#ifdef CONFIG_RSI_DEBUGFS
	/* In USB, one less than the MAX_DEBUGFS_ENTRIES entries is required */
	adapter->num_debugfs_entries = (MAX_DEBUGFS_ENTRIES - 1);
#endif

	rsi_dbg(INIT_ZONE, "%s: Enabled the interface\n", __func__);
	return 0;

fail_rx:
	kfree(rsi_dev->tx_buffer);

fail_eps:

	return status;
}