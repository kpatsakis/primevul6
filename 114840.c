static int curl_debug_cb(__maybe_unused CURL *handle, curl_infotype type,
			 char *data, size_t size,
			 void *userdata)
{
	struct pool *pool = (struct pool *)userdata;

	switch(type) {
		case CURLINFO_HEADER_IN:
		case CURLINFO_DATA_IN:
		case CURLINFO_SSL_DATA_IN:
			pool->cgminer_pool_stats.bytes_received += size;
			total_bytes_xfer += size;
			pool->cgminer_pool_stats.net_bytes_received += size;
			break;
		case CURLINFO_HEADER_OUT:
		case CURLINFO_DATA_OUT:
		case CURLINFO_SSL_DATA_OUT:
			pool->cgminer_pool_stats.bytes_sent += size;
			total_bytes_xfer += size;
			pool->cgminer_pool_stats.net_bytes_sent += size;
			break;
		case CURLINFO_TEXT:
		{
			if (!opt_protocol)
				break;
			// data is not null-terminated, so we need to copy and terminate it for applog
			char datacp[size + 1];
			memcpy(datacp, data, size);
			while (isspace(datacp[size-1]))
				--size;
			datacp[size] = '\0';
			applog(LOG_DEBUG, "Pool %u: %s", pool->pool_no, datacp);
			break;
		}
		default:
			break;
	}
	return 0;
}