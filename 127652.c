static int aac_get_hba_info(struct aac_dev *dev, void __user *arg)
{
	struct aac_hba_info hbainfo;

	memset(&hbainfo, 0, sizeof(hbainfo));
	hbainfo.adapter_number		= (u8) dev->id;
	hbainfo.system_io_bus_number	= dev->pdev->bus->number;
	hbainfo.device_number		= (dev->pdev->devfn >> 3);
	hbainfo.function_number		= (dev->pdev->devfn & 0x0007);

	hbainfo.vendor_id		= dev->pdev->vendor;
	hbainfo.device_id		= dev->pdev->device;
	hbainfo.sub_vendor_id		= dev->pdev->subsystem_vendor;
	hbainfo.sub_system_id		= dev->pdev->subsystem_device;

	if (copy_to_user(arg, &hbainfo, sizeof(struct aac_hba_info))) {
		dprintk((KERN_DEBUG "aacraid: Could not copy hba info\n"));
		return -EFAULT;
	}

	return 0;
}