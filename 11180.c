static int elo_flush_smartset_responses(struct usb_device *dev)
{
	return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
			ELO_FLUSH_SMARTSET_RESPONSES,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0, 0, NULL, 0, USB_CTRL_SET_TIMEOUT);
}