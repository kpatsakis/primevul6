GF_Err kind_Read(GF_Box *s,GF_BitStream *bs)
{
	GF_KindBox *ptr = (GF_KindBox *)s;

	if (ptr->size) {
		u32 bytesToRead = (u32) ptr->size;
		char *data;
		u32 schemeURIlen;
		data = (char*)gf_malloc(bytesToRead * sizeof(char));
		if (data == NULL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, data, bytesToRead);
		/*safety check in case the string is not null-terminated*/
		if (data[bytesToRead-1]) {
			char *str = (char*)gf_malloc((u32) bytesToRead + 1);
			memcpy(str, data, (u32) bytesToRead);
			str[ptr->size] = 0;
			gf_free(data);
			data = str;
			bytesToRead++;
		}
		ptr->schemeURI = gf_strdup(data);
		schemeURIlen = (u32) strlen(data);
		if (bytesToRead > schemeURIlen+1) {
			/* read the value */
			char *data_value = data + schemeURIlen +1;
			ptr->value = gf_strdup(data_value);
		}
		gf_free(data);
	}
	return GF_OK;
}