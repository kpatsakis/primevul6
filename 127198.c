wf_cliprdr_server_file_contents_response(CliprdrClientContext* context,
                                         const CLIPRDR_FILE_CONTENTS_RESPONSE* fileContentsResponse)
{
	wfClipboard* clipboard;

	if (!context || !fileContentsResponse)
		return ERROR_INTERNAL_ERROR;

	if (fileContentsResponse->msgFlags != CB_RESPONSE_OK)
		return E_FAIL;

	clipboard = (wfClipboard*)context->custom;

	if (!clipboard)
		return ERROR_INTERNAL_ERROR;

	clipboard->req_fsize = fileContentsResponse->cbRequested;
	clipboard->req_fdata = (char*)malloc(fileContentsResponse->cbRequested);

	if (!clipboard->req_fdata)
		return ERROR_INTERNAL_ERROR;

	CopyMemory(clipboard->req_fdata, fileContentsResponse->requestedData,
	           fileContentsResponse->cbRequested);

	if (!SetEvent(clipboard->req_fevent))
	{
		free(clipboard->req_fdata);
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;
}