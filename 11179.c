static void elo_process_data(struct input_dev *input, const u8 *data, int size)
{
	int press;

	input_report_abs(input, ABS_X, (data[3] << 8) | data[2]);
	input_report_abs(input, ABS_Y, (data[5] << 8) | data[4]);

	press = 0;
	if (data[1] & 0x80)
		press = (data[7] << 8) | data[6];
	input_report_abs(input, ABS_PRESSURE, press);

	if (data[1] & 0x03) {
		input_report_key(input, BTN_TOUCH, 1);
		input_sync(input);
	}

	if (data[1] & 0x04)
		input_report_key(input, BTN_TOUCH, 0);

	input_sync(input);
}