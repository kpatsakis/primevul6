httpd_senddone(isc_nmhandle_t *handle, isc_result_t result, void *arg) {
	isc_httpd_t *httpd = (isc_httpd_t *)arg;

	REQUIRE(VALID_HTTPD(httpd));
	REQUIRE(httpd->state == SEND);
	REQUIRE(httpd->handle == handle);

	isc_buffer_free(&httpd->sendbuffer);

	/*
	 * We will always want to clean up our receive buffer, even if we
	 * got an error on send or we are shutting down.
	 */
	if (httpd->freecb != NULL) {
		isc_buffer_t *b = NULL;
		if (isc_buffer_length(&httpd->bodybuffer) > 0) {
			b = &httpd->bodybuffer;
			httpd->freecb(b, httpd->freecb_arg);
		}
	}

	isc_nmhandle_detach(&httpd->sendhandle);

	if (result != ISC_R_SUCCESS) {
		goto cleanup_readhandle;
	}

	if ((httpd->flags & HTTPD_CLOSE) != 0) {
		goto cleanup_readhandle;
	}

	httpd->state = RECV;
	httpd->sendhandle = NULL;

	if (httpd->recvlen != 0) {
		/*
		 * Outstanding requests still exist, start processing
		 * them.
		 */
		httpd_request(httpd->handle, ISC_R_SUCCESS, NULL, httpd->mgr);
	} else if (!httpd->truncated) {
		isc_nm_resumeread(httpd->readhandle);
	} else {
		/* Truncated request, don't resume */
		goto cleanup_readhandle;
	}

	return;

cleanup_readhandle:
	isc_nmhandle_detach(&httpd->readhandle);
}