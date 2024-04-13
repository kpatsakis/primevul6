static void add_text(char **buffer, u32 *size, u32 *pos, char *msg, u32 msg_len)
{
	if (!msg || !buffer) return;

	if (*pos+msg_len>*size) {
		*size = *pos+msg_len-*size+256;
		*buffer = (char *)gf_realloc(*buffer, *size);
	}
	strncpy((*buffer)+(*pos), msg, msg_len);
	*pos += msg_len;
}