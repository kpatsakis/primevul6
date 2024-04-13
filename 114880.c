static const char *WindowsErrorStr(DWORD dwMessageId)
{
	static LPSTR msg = NULL;
	if (msg)
		LocalFree(msg);
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, dwMessageId, 0, (LPSTR)&msg, 0, 0))
		return msg;
	static const char fmt[] = "Error #%ld";
	signed long ldMsgId = dwMessageId;
	int sz = snprintf((char*)&sz, 0, fmt, ldMsgId) + 1;
	msg = (LPTSTR)LocalAlloc(LMEM_FIXED, sz);
	sprintf((char*)msg, fmt, ldMsgId);
	return msg;
}