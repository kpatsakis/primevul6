void reply_findnclose(struct smb_request *req)
{
	int dptr_num;

	START_PROFILE(SMBfindnclose);

	if (req->wct < 1) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBfindnclose);
		return;
	}

	dptr_num = SVAL(req->vwv+0, 0);

	DEBUG(3,("reply_findnclose, dptr_num = %d\n", dptr_num));

	/* We never give out valid handles for a 
	   findnotifyfirst - so any dptr_num is ok here. 
	   Just ignore it. */

	reply_outbuf(req, 0, 0);

	DEBUG(3,("SMB_findnclose dptr_num = %d\n", dptr_num));

	END_PROFILE(SMBfindnclose);
	return;
}