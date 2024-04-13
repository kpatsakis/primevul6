static int update_event_timestamp(RADIUS_PACKET *packet, time_t when)
{
	VALUE_PAIR *vp;

	vp = pairfind(packet->vps, PW_EVENT_TIMESTAMP);
	if (!vp) return 0;

	vp->vp_date = when;

	if (packet->data) {
		free(packet->data);
		packet->data = NULL;
		packet->data_len = 0;
	}

	return 1;		/* time stamp updated */
}