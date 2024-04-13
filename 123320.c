httpd_request(isc_nmhandle_t *handle, isc_result_t eresult,
	      isc_region_t *region, void *arg) {
	isc_result_t result;
	isc_httpd_t *httpd = NULL;
	isc_httpdmgr_t *mgr = (isc_httpdmgr_t *)arg;
	isc_buffer_t *databuffer = NULL;
	isc_httpdurl_t *url = NULL;
	isc_time_t now;
	isc_region_t r;
	bool is_compressed = false;
	char datebuf[ISC_FORMATHTTPTIMESTAMP_SIZE];
	size_t buflen = 0;

	httpd = isc_nmhandle_getdata(handle);

	REQUIRE(httpd->state == RECV);
	REQUIRE(httpd->handle == handle);

	if (eresult != ISC_R_SUCCESS) {
		goto cleanup_readhandle;
	}

	result = process_request(
		httpd, region == NULL ? &(isc_region_t){ NULL, 0 } : region,
		&buflen);
	if (result == ISC_R_NOTFOUND) {
		if (buflen < HTTP_RECVLEN - 1) {
			if (region != NULL) {
				/* don't unref, keep reading */
				return;
			}

			/*
			 * We must have been called from httpd_senddone (as
			 * ISC_R_NOTFOUND is not returned from netmgr) and we
			 * need to resume reading.
			 */
			isc_nm_resumeread(httpd->readhandle);
			return;
		}
		goto cleanup_readhandle;
	} else if (result != ISC_R_SUCCESS) {
		goto cleanup_readhandle;
	}

	isc_buffer_initnull(&httpd->bodybuffer);
	isc_time_now(&now);
	isc_time_formathttptimestamp(&now, datebuf, sizeof(datebuf));

	LOCK(&mgr->lock);
	url = ISC_LIST_HEAD(mgr->urls);
	while (url != NULL) {
		if (strcmp(httpd->url, url->url) == 0) {
			break;
		}
		url = ISC_LIST_NEXT(url, link);
	}
	UNLOCK(&mgr->lock);

	if (url == NULL) {
		result = mgr->render_404(
			httpd->url, NULL, httpd->querystring, NULL, NULL,
			&httpd->retcode, &httpd->retmsg, &httpd->mimetype,
			&httpd->bodybuffer, &httpd->freecb, &httpd->freecb_arg);
	} else {
		result = url->action(httpd->url, url, httpd->querystring,
				     httpd->headers, url->action_arg,
				     &httpd->retcode, &httpd->retmsg,
				     &httpd->mimetype, &httpd->bodybuffer,
				     &httpd->freecb, &httpd->freecb_arg);
	}
	if (result != ISC_R_SUCCESS) {
		result = mgr->render_500(
			httpd->url, url, httpd->querystring, NULL, NULL,
			&httpd->retcode, &httpd->retmsg, &httpd->mimetype,
			&httpd->bodybuffer, &httpd->freecb, &httpd->freecb_arg);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
	}

#ifdef HAVE_ZLIB
	if ((httpd->flags & HTTPD_ACCEPT_DEFLATE) != 0) {
		result = httpd_compress(httpd);
		if (result == ISC_R_SUCCESS) {
			is_compressed = true;
		}
	}
#endif /* ifdef HAVE_ZLIB */

	httpd_response(httpd);
	if ((httpd->flags & HTTPD_KEEPALIVE) != 0) {
		httpd_addheader(httpd, "Connection", "Keep-Alive");
	}
	httpd_addheader(httpd, "Content-Type", httpd->mimetype);
	httpd_addheader(httpd, "Date", datebuf);
	httpd_addheader(httpd, "Expires", datebuf);

	if (url != NULL && url->isstatic) {
		char loadbuf[ISC_FORMATHTTPTIMESTAMP_SIZE];
		isc_time_formathttptimestamp(&url->loadtime, loadbuf,
					     sizeof(loadbuf));
		httpd_addheader(httpd, "Last-Modified", loadbuf);
		httpd_addheader(httpd, "Cache-Control: public", NULL);
	} else {
		httpd_addheader(httpd, "Last-Modified", datebuf);
		httpd_addheader(httpd, "Pragma: no-cache", NULL);
		httpd_addheader(httpd, "Cache-Control: no-cache", NULL);
	}

	httpd_addheader(httpd, "Server: libisc", NULL);

	if (is_compressed) {
		httpd_addheader(httpd, "Content-Encoding", "deflate");
		httpd_addheaderuint(httpd, "Content-Length",
				    isc_buffer_usedlength(&httpd->compbuffer));
	} else {
		httpd_addheaderuint(httpd, "Content-Length",
				    isc_buffer_usedlength(&httpd->bodybuffer));
	}

	httpd_endheaders(httpd); /* done */

	/*
	 * Append either the compressed or the non-compressed response body to
	 * the response headers and store the result in httpd->sendbuffer.
	 */
	isc_buffer_dup(mgr->mctx, &httpd->sendbuffer, &httpd->headerbuffer);
	isc_buffer_clear(&httpd->headerbuffer);
	isc_buffer_setautorealloc(httpd->sendbuffer, true);
	databuffer = (is_compressed ? &httpd->compbuffer : &httpd->bodybuffer);
	isc_buffer_usedregion(databuffer, &r);
	result = isc_buffer_copyregion(httpd->sendbuffer, &r);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	/* Consume the request from the recv buffer. */
	if (httpd->consume != 0U) {
		INSIST(httpd->consume <= httpd->recvlen);
		if (httpd->consume < httpd->recvlen) {
			memmove(httpd->recvbuf, httpd->recvbuf + httpd->consume,
				httpd->recvlen - httpd->consume);
		}
		httpd->recvlen -= httpd->consume;
		httpd->consume = 0;
		httpd->recvbuf[httpd->recvlen] = 0;
	}

	/*
	 * Determine total response size.
	 */
	isc_buffer_usedregion(httpd->sendbuffer, &r);

	isc_nm_pauseread(httpd->handle);
	httpd->state = SEND;

	isc_nmhandle_attach(httpd->handle, &httpd->sendhandle);
	isc_nm_send(httpd->sendhandle, &r, httpd_senddone, httpd);
	return;

cleanup_readhandle:
	isc_nmhandle_detach(&httpd->readhandle);
}