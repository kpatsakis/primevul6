AppendStrToSelectionBuffer(SelectedCells * scp, Char *text, size_t len)
{
    if (len != 0) {
	size_t j = (scp->data_length + len);
	size_t k = j + (j >> 2) + 80;
	if (j + 1 >= scp->data_limit) {
	    Char *line;
	    if (!scp->data_length) {
		line = (Char *) malloc(k);
	    } else {
		line = (Char *) realloc(scp->data_buffer, k);
	    }
	    if (line == 0)
		SysError(ERROR_BMALLOC2);
	    scp->data_buffer = line;
	    scp->data_limit = k;
	}
	if (scp->data_buffer != 0) {
	    memcpy(scp->data_buffer + scp->data_length, text, len);
	    scp->data_length += len;
	    scp->data_buffer[scp->data_length] = 0;
	}
    }
}