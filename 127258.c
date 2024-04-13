wf_cliprdr_server_format_data_request(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest)
{
	UINT rc;
	size_t size = 0;
	void* buff = NULL;
	char* globlemem = NULL;
	HANDLE hClipdata = NULL;
	UINT32 requestedFormatId;
	CLIPRDR_FORMAT_DATA_RESPONSE response;
	wfClipboard* clipboard;

	if (!context || !formatDataRequest)
		return ERROR_INTERNAL_ERROR;

	clipboard = (wfClipboard*)context->custom;

	if (!clipboard)
		return ERROR_INTERNAL_ERROR;

	requestedFormatId = formatDataRequest->requestedFormatId;

	if (requestedFormatId == RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW))
	{
		size_t len;
		size_t i;
		WCHAR* wFileName;
		HRESULT result;
		LPDATAOBJECT dataObj;
		FORMATETC format_etc;
		STGMEDIUM stg_medium;
		DROPFILES* dropFiles;
		FILEGROUPDESCRIPTORW* groupDsc;
		result = OleGetClipboard(&dataObj);

		if (FAILED(result))
			return ERROR_INTERNAL_ERROR;

		ZeroMemory(&format_etc, sizeof(FORMATETC));
		ZeroMemory(&stg_medium, sizeof(STGMEDIUM));
		/* get DROPFILES struct from OLE */
		format_etc.cfFormat = CF_HDROP;
		format_etc.tymed = TYMED_HGLOBAL;
		format_etc.dwAspect = 1;
		format_etc.lindex = -1;
		result = IDataObject_GetData(dataObj, &format_etc, &stg_medium);

		if (FAILED(result))
		{
			DEBUG_CLIPRDR("dataObj->GetData failed.");
			goto exit;
		}

		dropFiles = (DROPFILES*)GlobalLock(stg_medium.hGlobal);

		if (!dropFiles)
		{
			GlobalUnlock(stg_medium.hGlobal);
			ReleaseStgMedium(&stg_medium);
			clipboard->nFiles = 0;
			goto exit;
		}

		clear_file_array(clipboard);

		if (dropFiles->fWide)
		{
			/* dropFiles contains file names */
			for (wFileName = (WCHAR*)((char*)dropFiles + dropFiles->pFiles);
			     (len = wcslen(wFileName)) > 0; wFileName += len + 1)
			{
				wf_cliprdr_process_filename(clipboard, wFileName, wcslen(wFileName));
			}
		}
		else
		{
			char* p;

			for (p = (char*)((char*)dropFiles + dropFiles->pFiles); (len = strlen(p)) > 0;
			     p += len + 1, clipboard->nFiles++)
			{
				int cchWideChar;
				WCHAR* wFileName;
				cchWideChar = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, p, len, NULL, 0);
				wFileName = (LPWSTR)calloc(cchWideChar, sizeof(WCHAR));
				MultiByteToWideChar(CP_ACP, MB_COMPOSITE, p, len, wFileName, cchWideChar);
				wf_cliprdr_process_filename(clipboard, wFileName, cchWideChar);
			}
		}

		GlobalUnlock(stg_medium.hGlobal);
		ReleaseStgMedium(&stg_medium);
	exit:
		size = 4 + clipboard->nFiles * sizeof(FILEDESCRIPTORW);
		groupDsc = (FILEGROUPDESCRIPTORW*)malloc(size);

		if (groupDsc)
		{
			groupDsc->cItems = clipboard->nFiles;

			for (i = 0; i < clipboard->nFiles; i++)
			{
				if (clipboard->fileDescriptor[i])
					groupDsc->fgd[i] = *clipboard->fileDescriptor[i];
			}

			buff = groupDsc;
		}

		IDataObject_Release(dataObj);
	}
	else
	{
		/* Ignore if other app is holding the clipboard */
		if (try_open_clipboard(clipboard->hwnd))
		{
			hClipdata = GetClipboardData(requestedFormatId);

			if (!hClipdata)
			{
				CloseClipboard();
				return ERROR_INTERNAL_ERROR;
			}

			globlemem = (char*)GlobalLock(hClipdata);
			size = (int)GlobalSize(hClipdata);
			buff = malloc(size);
			CopyMemory(buff, globlemem, size);
			GlobalUnlock(hClipdata);
			CloseClipboard();
		}
	}

	response.msgFlags = CB_RESPONSE_OK;
	response.dataLen = size;
	response.requestedFormatData = (BYTE*)buff;
	rc = clipboard->context->ClientFormatDataResponse(clipboard->context, &response);
	free(buff);
	return rc;
}