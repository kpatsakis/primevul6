static int avrcp_controller_server_probe(struct btd_profile *p,
						struct btd_adapter *adapter)
{
	sdp_record_t *record;
	struct avrcp_server *server;

	DBG("path %s", adapter_get_path(adapter));

	server = find_server(servers, adapter);
	if (server != NULL)
		goto done;

	server = avrcp_server_register(adapter);
	if (server == NULL)
		return -EPROTONOSUPPORT;

done:
	record = avrcp_ct_record();
	if (!record) {
		error("Unable to allocate new service record");
		avrcp_controller_server_remove(p, adapter);
		return -1;
	}

	if (adapter_service_add(adapter, record) < 0) {
		error("Unable to register AVRCP service record");
		avrcp_controller_server_remove(p, adapter);
		sdp_record_free(record);
		return -1;
	}
	server->ct_record_id = record->handle;

	return 0;
}