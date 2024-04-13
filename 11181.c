static int elo_smartset_send_get(struct usb_device *dev, u8 command,
		void *data)
{
	unsigned int pipe;
	u8 dir;

	if (command == ELO_SEND_SMARTSET_COMMAND) {
		pipe = usb_sndctrlpipe(dev, 0);
		dir = USB_DIR_OUT;
	} else if (command == ELO_GET_SMARTSET_RESPONSE) {
		pipe = usb_rcvctrlpipe(dev, 0);
		dir = USB_DIR_IN;
	} else
		return -EINVAL;

	return usb_control_msg(dev, pipe, command,
			dir | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0, 0, data, ELO_SMARTSET_PACKET_SIZE,
			ELO_SMARTSET_CMD_TIMEOUT);
}