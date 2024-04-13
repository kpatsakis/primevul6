static int elo_input_configured(struct hid_device *hdev,
		struct hid_input *hidinput)
{
	struct input_dev *input = hidinput->input;

	/*
	 * ELO devices have one Button usage in GenDesk field, which makes
	 * hid-input map it to BTN_LEFT; that confuses userspace, which then
	 * considers the device to be a mouse/touchpad instead of touchscreen.
	 */
	clear_bit(BTN_LEFT, input->keybit);
	set_bit(BTN_TOUCH, input->keybit);
	set_bit(ABS_PRESSURE, input->absbit);
	input_set_abs_params(input, ABS_PRESSURE, 0, 256, 0, 0);

	return 0;
}