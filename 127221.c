wf_cliprdr_server_format_list_response(CliprdrClientContext* context,
                                       const CLIPRDR_FORMAT_LIST_RESPONSE* formatListResponse)
{
	(void)context;
	(void)formatListResponse;

	if (formatListResponse->msgFlags != CB_RESPONSE_OK)
		return E_FAIL;

	return CHANNEL_RC_OK;
}