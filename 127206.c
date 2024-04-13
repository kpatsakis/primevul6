wf_cliprdr_server_format_data_response(CliprdrClientContext* context,
                                       const CLIPRDR_FORMAT_DATA_RESPONSE* formatDataResponse)
{
	BYTE* data;
	HANDLE hMem;
	wfClipboard* clipboard;

	if (!context || !formatDataResponse)
		return ERROR_INTERNAL_ERROR;

	if (formatDataResponse->msgFlags != CB_RESPONSE_OK)
		return E_FAIL;

	clipboard = (wfClipboard*)context->custom;

	if (!clipboard)
		return ERROR_INTERNAL_ERROR;

	hMem = GlobalAlloc(GMEM_MOVEABLE, formatDataResponse->dataLen);

	if (!hMem)
		return ERROR_INTERNAL_ERROR;

	data = (BYTE*)GlobalLock(hMem);

	if (!data)
	{
		GlobalFree(hMem);
		return ERROR_INTERNAL_ERROR;
	}

	CopyMemory(data, formatDataResponse->requestedFormatData, formatDataResponse->dataLen);

	if (!GlobalUnlock(hMem) && GetLastError())
	{
		GlobalFree(hMem);
		return ERROR_INTERNAL_ERROR;
	}

	clipboard->hmem = hMem;

	if (!SetEvent(clipboard->response_data_event))
		return ERROR_INTERNAL_ERROR;

	return CHANNEL_RC_OK;
}