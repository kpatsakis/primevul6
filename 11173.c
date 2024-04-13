static bool elo_broken_firmware(struct usb_device *dev)
{
	struct usb_device *hub = dev->parent;
	struct usb_device *child = NULL;
	u16 fw_lvl = le16_to_cpu(dev->descriptor.bcdDevice);
	u16 child_vid, child_pid;
	int i;
    
	if (!use_fw_quirk)
		return false;
	if (fw_lvl != 0x10d)
		return false;

	/* iterate sibling devices of the touch controller */
	usb_hub_for_each_child(hub, i, child) {
		child_vid = le16_to_cpu(child->descriptor.idVendor);
		child_pid = le16_to_cpu(child->descriptor.idProduct);

		/*
		 * If one of the devices below is present attached as a sibling of 
		 * the touch controller then  this is a newer IBM 4820 monitor that 
		 * does not need the IBM-requested workaround if fw level is
		 * 0x010d - aka 'M'.
		 * No other HW can have this combination.
		 */
		if (child_vid==0x04b3) {
			switch (child_pid) {
			case 0x4676: /* 4820 21x Video */
			case 0x4677: /* 4820 51x Video */
			case 0x4678: /* 4820 2Lx Video */
			case 0x4679: /* 4820 5Lx Video */
				return false;
			}
		}
	}
	return true;
}