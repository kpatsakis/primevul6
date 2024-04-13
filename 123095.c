static void remove_from_request_hash(REQUEST *request)
{
	if (!request->in_request_hash) return;

	fr_packet_list_yank(pl, request->packet);
	request->in_request_hash = FALSE;

	request_stats_final(request);
}