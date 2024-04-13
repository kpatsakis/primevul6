static int elo_raw_event(struct hid_device *hdev, struct hid_report *report,
	 u8 *data, int size)
{
	struct hid_input *hidinput;

	if (!(hdev->claimed & HID_CLAIMED_INPUT) || list_empty(&hdev->inputs))
		return 0;

	hidinput = list_first_entry(&hdev->inputs, struct hid_input, list);

	switch (report->id) {
	case 0:
		if (data[0] == 'T') {	/* Mandatory ELO packet marker */
			elo_process_data(hidinput->input, data, size);
			return 1;
		}
		break;
	default:	/* unknown report */
		/* Unknown report type; pass upstream */
		hid_info(hdev, "unknown report type %d\n", report->id);
		break;
	}

	return 0;
}