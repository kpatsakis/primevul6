static int pcan_usb_fd_init(struct peak_usb_device *dev)
{
	struct pcan_usb_fd_device *pdev =
			container_of(dev, struct pcan_usb_fd_device, dev);
	int i, err = -ENOMEM;

	/* do this for 1st channel only */
	if (!dev->prev_siblings) {
		/* allocate netdevices common structure attached to first one */
		pdev->usb_if = kzalloc(sizeof(*pdev->usb_if), GFP_KERNEL);
		if (!pdev->usb_if)
			goto err_out;

		/* allocate command buffer once for all for the interface */
		pdev->cmd_buffer_addr = kmalloc(PCAN_UFD_CMD_BUFFER_SIZE,
						GFP_KERNEL);
		if (!pdev->cmd_buffer_addr)
			goto err_out_1;

		/* number of ts msgs to ignore before taking one into account */
		pdev->usb_if->cm_ignore_count = 5;

		err = pcan_usb_pro_send_req(dev, PCAN_USBPRO_REQ_INFO,
					    PCAN_USBPRO_INFO_FW,
					    &pdev->usb_if->fw_info,
					    sizeof(pdev->usb_if->fw_info));
		if (err) {
			dev_err(dev->netdev->dev.parent,
				"unable to read %s firmware info (err %d)\n",
				dev->adapter->name, err);
			goto err_out_2;
		}

		/* explicit use of dev_xxx() instead of netdev_xxx() here:
		 * information displayed are related to the device itself, not
		 * to the canx (channel) device.
		 */
		dev_info(dev->netdev->dev.parent,
			 "PEAK-System %s v%u fw v%u.%u.%u (%u channels)\n",
			 dev->adapter->name, pdev->usb_if->fw_info.hw_version,
			 pdev->usb_if->fw_info.fw_version[0],
			 pdev->usb_if->fw_info.fw_version[1],
			 pdev->usb_if->fw_info.fw_version[2],
			 dev->adapter->ctrl_count);

		/* check for ability to switch between ISO/non-ISO modes */
		if (pdev->usb_if->fw_info.fw_version[0] >= 2) {
			/* firmware >= 2.x supports ISO/non-ISO switching */
			dev->can.ctrlmode_supported |= CAN_CTRLMODE_FD_NON_ISO;
		} else {
			/* firmware < 2.x only supports fixed(!) non-ISO */
			dev->can.ctrlmode |= CAN_CTRLMODE_FD_NON_ISO;
		}

		/* tell the hardware the can driver is running */
		err = pcan_usb_fd_drv_loaded(dev, 1);
		if (err) {
			dev_err(dev->netdev->dev.parent,
				"unable to tell %s driver is loaded (err %d)\n",
				dev->adapter->name, err);
			goto err_out_2;
		}
	} else {
		/* otherwise, simply copy previous sibling's values */
		struct pcan_usb_fd_device *ppdev =
			container_of(dev->prev_siblings,
				     struct pcan_usb_fd_device, dev);

		pdev->usb_if = ppdev->usb_if;
		pdev->cmd_buffer_addr = ppdev->cmd_buffer_addr;

		/* do a copy of the ctrlmode[_supported] too */
		dev->can.ctrlmode = ppdev->dev.can.ctrlmode;
		dev->can.ctrlmode_supported = ppdev->dev.can.ctrlmode_supported;
	}

	pdev->usb_if->dev[dev->ctrl_idx] = dev;
	dev->device_number =
		le32_to_cpu(pdev->usb_if->fw_info.dev_id[dev->ctrl_idx]);

	/* set clock domain */
	for (i = 0; i < ARRAY_SIZE(pcan_usb_fd_clk_freq); i++)
		if (dev->adapter->clock.freq == pcan_usb_fd_clk_freq[i])
			break;

	if (i >= ARRAY_SIZE(pcan_usb_fd_clk_freq)) {
		dev_warn(dev->netdev->dev.parent,
			 "incompatible clock frequencies\n");
		err = -EINVAL;
		goto err_out_2;
	}

	pcan_usb_fd_set_clock_domain(dev, i);

	/* set LED in default state (end of init phase) */
	pcan_usb_fd_set_can_led(dev, PCAN_UFD_LED_DEF);

	return 0;

err_out_2:
	kfree(pdev->cmd_buffer_addr);
err_out_1:
	kfree(pdev->usb_if);
err_out:
	return err;
}