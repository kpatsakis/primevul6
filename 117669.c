static CURLcode single_transfer(struct GlobalConfig *global,
                                struct OperationConfig *config,
                                CURLSH *share,
                                bool capath_from_env,
                                bool *added)
{
  CURLcode result = CURLE_OK;
  struct getout *urlnode;
  bool orig_noprogress = global->noprogress;
  bool orig_isatty = global->isatty;
  struct State *state = &config->state;
  char *httpgetfields = state->httpgetfields;
  *added = FALSE; /* not yet */

  if(config->postfields) {
    if(config->use_httpget) {
      if(!httpgetfields) {
        /* Use the postfields data for a http get */
        httpgetfields = state->httpgetfields = strdup(config->postfields);
        Curl_safefree(config->postfields);
        if(!httpgetfields) {
          errorf(global, "out of memory\n");
          result = CURLE_OUT_OF_MEMORY;
        }
        else if(SetHTTPrequest(config,
                               (config->no_body?HTTPREQ_HEAD:HTTPREQ_GET),
                               &config->httpreq)) {
          result = CURLE_FAILED_INIT;
        }
      }
    }
    else {
      if(SetHTTPrequest(config, HTTPREQ_SIMPLEPOST, &config->httpreq))
        result = CURLE_FAILED_INIT;
    }
    if(result) {
      single_transfer_cleanup(config);
      return result;
    }
  }
  if(!state->urlnode) {
    /* first time caller, setup things */
    state->urlnode = config->url_list;
    state->infilenum = 1;
  }

  while(config->state.urlnode) {
    static bool warn_more_options = FALSE;
    char *infiles; /* might be a glob pattern */
    struct URLGlob *inglob = state->inglob;
    urlnode = config->state.urlnode;

    /* urlnode->url is the full URL (it might be NULL) */

    if(!urlnode->url) {
      /* This node has no URL. Free node data without destroying the
         node itself nor modifying next pointer and continue to next */
      Curl_safefree(urlnode->outfile);
      Curl_safefree(urlnode->infile);
      urlnode->flags = 0;
      config->state.urlnode = urlnode->next;
      state->up = 0;
      if(!warn_more_options) {
        /* only show this once */
        warnf(config->global, "Got more output options than URLs\n");
        warn_more_options = TRUE;
      }
      continue; /* next URL please */
    }

    /* save outfile pattern before expansion */
    if(urlnode->outfile && !state->outfiles) {
      state->outfiles = strdup(urlnode->outfile);
      if(!state->outfiles) {
        errorf(global, "out of memory\n");
        result = CURLE_OUT_OF_MEMORY;
        break;
      }
    }

    infiles = urlnode->infile;

    if(!config->globoff && infiles && !inglob) {
      /* Unless explicitly shut off */
      result = glob_url(&inglob, infiles, &state->infilenum,
                        global->showerror?global->errors:NULL);
      if(result)
        break;
      config->state.inglob = inglob;
    }

    {
      unsigned long urlnum;

      if(!state->up && !infiles)
        Curl_nop_stmt;
      else {
        if(!state->uploadfile) {
          if(inglob) {
            result = glob_next_url(&state->uploadfile, inglob);
            if(result == CURLE_OUT_OF_MEMORY)
              errorf(global, "out of memory\n");
          }
          else if(!state->up) {
            state->uploadfile = strdup(infiles);
            if(!state->uploadfile) {
              errorf(global, "out of memory\n");
              result = CURLE_OUT_OF_MEMORY;
            }
          }
        }
        if(result)
          break;
      }

      if(!state->urlnum) {
        if(!config->globoff) {
          /* Unless explicitly shut off, we expand '{...}' and '[...]'
             expressions and return total number of URLs in pattern set */
          result = glob_url(&state->urls, urlnode->url, &state->urlnum,
                            global->showerror?global->errors:NULL);
          if(result)
            break;
          urlnum = state->urlnum;
        }
        else
          urlnum = 1; /* without globbing, this is a single URL */
      }
      else
        urlnum = state->urlnum;

      if(state->up < state->infilenum) {
        struct per_transfer *per = NULL;
        struct OutStruct *outs;
        struct InStruct *input;
        struct OutStruct *heads;
        struct OutStruct *etag_save;
        struct HdrCbData *hdrcbdata = NULL;
        struct OutStruct etag_first;
        long use_proto;
        CURL *curl;

        /* --etag-save */
        memset(&etag_first, 0, sizeof(etag_first));
        etag_save = &etag_first;
        etag_save->stream = stdout;

        /* --etag-compare */
        if(config->etag_compare_file) {
          char *etag_from_file = NULL;
          char *header = NULL;
          ParameterError pe;

          /* open file for reading: */
          FILE *file = fopen(config->etag_compare_file, FOPEN_READTEXT);
          if(!file && !config->etag_save_file) {
            errorf(global,
                   "Failed to open %s\n", config->etag_compare_file);
            result = CURLE_READ_ERROR;
            break;
          }

          if((PARAM_OK == file2string(&etag_from_file, file)) &&
             etag_from_file) {
            header = aprintf("If-None-Match: %s", etag_from_file);
            Curl_safefree(etag_from_file);
          }
          else
            header = aprintf("If-None-Match: \"\"");

          if(!header) {
            if(file)
              fclose(file);
            errorf(global,
                   "Failed to allocate memory for custom etag header\n");
            result = CURLE_OUT_OF_MEMORY;
            break;
          }

          /* add Etag from file to list of custom headers */
          pe = add2list(&config->headers, header);
          Curl_safefree(header);

          if(file)
            fclose(file);
          if(pe != PARAM_OK) {
            result = CURLE_OUT_OF_MEMORY;
            break;
          }
        }

        if(config->etag_save_file) {
          /* open file for output: */
          if(strcmp(config->etag_save_file, "-")) {
            FILE *newfile = fopen(config->etag_save_file, "wb");
            if(!newfile) {
              warnf(global, "Failed creating file for saving etags: \"%s\". "
                    "Skip this transfer\n", config->etag_save_file);
              Curl_safefree(state->outfiles);
              glob_cleanup(state->urls);
              return CURLE_OK;
            }
            else {
              etag_save->filename = config->etag_save_file;
              etag_save->s_isreg = TRUE;
              etag_save->fopened = TRUE;
              etag_save->stream = newfile;
            }
          }
          else {
            /* always use binary mode for protocol header output */
            set_binmode(etag_save->stream);
          }
        }

        curl = curl_easy_init();
        if(curl)
          result = add_per_transfer(&per);
        else
          result = CURLE_OUT_OF_MEMORY;
        if(result) {
          curl_easy_cleanup(curl);
          if(etag_save->fopened)
            fclose(etag_save->stream);
          break;
        }
        per->etag_save = etag_first; /* copy the whole struct */
        if(state->uploadfile) {
          per->uploadfile = strdup(state->uploadfile);
          if(!per->uploadfile) {
            curl_easy_cleanup(curl);
            result = CURLE_OUT_OF_MEMORY;
            break;
          }
          if(SetHTTPrequest(config, HTTPREQ_PUT, &config->httpreq)) {
            Curl_safefree(per->uploadfile);
            curl_easy_cleanup(curl);
            result = CURLE_FAILED_INIT;
            break;
          }
        }
        *added = TRUE;
        per->config = config;
        per->curl = curl;
        per->urlnum = urlnode->num;

        /* default headers output stream is stdout */
        heads = &per->heads;
        heads->stream = stdout;

        /* Single header file for all URLs */
        if(config->headerfile) {
          /* open file for output: */
          if(strcmp(config->headerfile, "-")) {
            FILE *newfile;
            newfile = fopen(config->headerfile, per->prev == NULL?"wb":"ab");
            if(!newfile) {
              warnf(global, "Failed to open %s\n", config->headerfile);
              result = CURLE_WRITE_ERROR;
              break;
            }
            else {
              heads->filename = config->headerfile;
              heads->s_isreg = TRUE;
              heads->fopened = TRUE;
              heads->stream = newfile;
            }
          }
          else {
            /* always use binary mode for protocol header output */
            set_binmode(heads->stream);
          }
        }

        hdrcbdata = &per->hdrcbdata;

        outs = &per->outs;
        input = &per->input;

        per->outfile = NULL;
        per->infdopen = FALSE;
        per->infd = STDIN_FILENO;

        /* default output stream is stdout */
        outs->stream = stdout;

        if(state->urls) {
          result = glob_next_url(&per->this_url, state->urls);
          if(result)
            break;
        }
        else if(!state->li) {
          per->this_url = strdup(urlnode->url);
          if(!per->this_url) {
            result = CURLE_OUT_OF_MEMORY;
            break;
          }
        }
        else
          per->this_url = NULL;
        if(!per->this_url)
          break;

        if(state->outfiles) {
          per->outfile = strdup(state->outfiles);
          if(!per->outfile) {
            result = CURLE_OUT_OF_MEMORY;
            break;
          }
        }

        if(((urlnode->flags&GETOUT_USEREMOTE) ||
            (per->outfile && strcmp("-", per->outfile)))) {

          /*
           * We have specified a file name to store the result in, or we have
           * decided we want to use the remote file name.
           */

          if(!per->outfile) {
            /* extract the file name from the URL */
            result = get_url_file_name(&per->outfile, per->this_url);
            if(result) {
              errorf(global, "Failed to extract a sensible file name"
                     " from the URL to use for storage!\n");
              break;
            }
            if(!*per->outfile && !config->content_disposition) {
              errorf(global, "Remote file name has no length!\n");
              result = CURLE_WRITE_ERROR;
              break;
            }
          }
          else if(state->urls) {
            /* fill '#1' ... '#9' terms from URL pattern */
            char *storefile = per->outfile;
            result = glob_match_url(&per->outfile, storefile, state->urls);
            Curl_safefree(storefile);
            if(result) {
              /* bad globbing */
              warnf(global, "bad output glob!\n");
              break;
            }
            if(!*per->outfile) {
              warnf(global, "output glob produces empty string!\n");
              result = CURLE_WRITE_ERROR;
              break;
            }
          }

          if(config->output_dir && *config->output_dir) {
            char *d = aprintf("%s/%s", config->output_dir, per->outfile);
            if(!d) {
              result = CURLE_WRITE_ERROR;
              break;
            }
            free(per->outfile);
            per->outfile = d;
          }
          /* Create the directory hierarchy, if not pre-existent to a multiple
             file output call */

          if(config->create_dirs) {
            result = create_dir_hierarchy(per->outfile, global->errors);
            /* create_dir_hierarchy shows error upon CURLE_WRITE_ERROR */
            if(result)
              break;
          }

          if((urlnode->flags & GETOUT_USEREMOTE)
             && config->content_disposition) {
            /* Our header callback MIGHT set the filename */
            DEBUGASSERT(!outs->filename);
          }

          if(config->resume_from_current) {
            /* We're told to continue from where we are now. Get the size
               of the file as it is now and open it for append instead */
            struct_stat fileinfo;
            /* VMS -- Danger, the filesize is only valid for stream files */
            if(0 == stat(per->outfile, &fileinfo))
              /* set offset to current file size: */
              config->resume_from = fileinfo.st_size;
            else
              /* let offset be 0 */
              config->resume_from = 0;
          }

          if(config->resume_from) {
#ifdef __VMS
            /* open file for output, forcing VMS output format into stream
               mode which is needed for stat() call above to always work. */
            FILE *file = fopen(outfile, "ab",
                               "ctx=stm", "rfm=stmlf", "rat=cr", "mrs=0");
#else
            /* open file for output: */
            FILE *file = fopen(per->outfile, "ab");
#endif
            if(!file) {
              errorf(global, "Can't open '%s'!\n", per->outfile);
              result = CURLE_WRITE_ERROR;
              break;
            }
            outs->fopened = TRUE;
            outs->stream = file;
            outs->init = config->resume_from;
          }
          else {
            outs->stream = NULL; /* open when needed */
          }
          outs->filename = per->outfile;
          outs->s_isreg = TRUE;
        }

        if(per->uploadfile && !stdin_upload(per->uploadfile)) {
          /*
           * We have specified a file to upload and it isn't "-".
           */
          char *nurl = add_file_name_to_url(per->this_url, per->uploadfile);
          if(!nurl) {
            result = CURLE_OUT_OF_MEMORY;
            break;
          }
          per->this_url = nurl;
        }
        else if(per->uploadfile && stdin_upload(per->uploadfile)) {
          /* count to see if there are more than one auth bit set
             in the authtype field */
          int authbits = 0;
          int bitcheck = 0;
          while(bitcheck < 32) {
            if(config->authtype & (1UL << bitcheck++)) {
              authbits++;
              if(authbits > 1) {
                /* more than one, we're done! */
                break;
              }
            }
          }

          /*
           * If the user has also selected --anyauth or --proxy-anyauth
           * we should warn him/her.
           */
          if(config->proxyanyauth || (authbits>1)) {
            warnf(global,
                  "Using --anyauth or --proxy-anyauth with upload from stdin"
                  " involves a big risk of it not working. Use a temporary"
                  " file or a fixed auth type instead!\n");
          }

          DEBUGASSERT(per->infdopen == FALSE);
          DEBUGASSERT(per->infd == STDIN_FILENO);

          set_binmode(stdin);
          if(!strcmp(per->uploadfile, ".")) {
            if(curlx_nonblock((curl_socket_t)per->infd, TRUE) < 0)
              warnf(global,
                    "fcntl failed on fd=%d: %s\n", per->infd, strerror(errno));
          }
        }

        if(per->uploadfile && config->resume_from_current)
          config->resume_from = -1; /* -1 will then force get-it-yourself */

        if(output_expected(per->this_url, per->uploadfile) && outs->stream &&
           isatty(fileno(outs->stream)))
          /* we send the output to a tty, therefore we switch off the progress
             meter */
          per->noprogress = global->noprogress = global->isatty = TRUE;
        else {
          /* progress meter is per download, so restore config
             values */
          per->noprogress = global->noprogress = orig_noprogress;
          global->isatty = orig_isatty;
        }

        if(httpgetfields) {
          char *urlbuffer;
          /* Find out whether the url contains a file name */
          const char *pc = strstr(per->this_url, "://");
          char sep = '?';
          if(pc)
            pc += 3;
          else
            pc = per->this_url;

          pc = strrchr(pc, '/'); /* check for a slash */

          if(pc) {
            /* there is a slash present in the URL */

            if(strchr(pc, '?'))
              /* Ouch, there's already a question mark in the URL string, we
                 then append the data with an ampersand separator instead! */
              sep = '&';
          }
          /*
           * Then append ? followed by the get fields to the url.
           */
          if(pc)
            urlbuffer = aprintf("%s%c%s", per->this_url, sep, httpgetfields);
          else
            /* Append  / before the ? to create a well-formed url
               if the url contains a hostname only
            */
            urlbuffer = aprintf("%s/?%s", per->this_url, httpgetfields);

          if(!urlbuffer) {
            result = CURLE_OUT_OF_MEMORY;
            break;
          }

          Curl_safefree(per->this_url); /* free previous URL */
          per->this_url = urlbuffer; /* use our new URL instead! */
        }

        if(!global->errors)
          global->errors = stderr;

        if((!per->outfile || !strcmp(per->outfile, "-")) &&
           !config->use_ascii) {
          /* We get the output to stdout and we have not got the ASCII/text
             flag, then set stdout to be binary */
          set_binmode(stdout);
        }

        /* explicitly passed to stdout means okaying binary gunk */
        config->terminal_binary_ok =
          (per->outfile && !strcmp(per->outfile, "-"));

        /* Avoid having this setopt added to the --libcurl source output. */
        result = curl_easy_setopt(curl, CURLOPT_SHARE, share);
        if(result)
          break;

        /* here */
        use_proto = url_proto(per->this_url);
#if 0
        if(!(use_proto & built_in_protos)) {
          warnf(global, "URL is '%s' but no support for the scheme\n",
                per->this_url);
        }
#endif

        if(!config->tcp_nodelay)
          my_setopt(curl, CURLOPT_TCP_NODELAY, 0L);

        if(config->tcp_fastopen)
          my_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);

        /* where to store */
        my_setopt(curl, CURLOPT_WRITEDATA, per);
        my_setopt(curl, CURLOPT_INTERLEAVEDATA, per);

        /* what call to write */
        my_setopt(curl, CURLOPT_WRITEFUNCTION, tool_write_cb);

        /* for uploads */
        input->config = config;
        /* Note that if CURLOPT_READFUNCTION is fread (the default), then
         * lib/telnet.c will Curl_poll() on the input file descriptor
         * rather than calling the READFUNCTION at regular intervals.
         * The circumstances in which it is preferable to enable this
         * behavior, by omitting to set the READFUNCTION & READDATA options,
         * have not been determined.
         */
        my_setopt(curl, CURLOPT_READDATA, input);
        /* what call to read */
        my_setopt(curl, CURLOPT_READFUNCTION, tool_read_cb);

        /* in 7.18.0, the CURLOPT_SEEKFUNCTION/DATA pair is taking over what
           CURLOPT_IOCTLFUNCTION/DATA pair previously provided for seeking */
        my_setopt(curl, CURLOPT_SEEKDATA, input);
        my_setopt(curl, CURLOPT_SEEKFUNCTION, tool_seek_cb);

        if(config->recvpersecond &&
           (config->recvpersecond < BUFFER_SIZE))
          /* use a smaller sized buffer for better sleeps */
          my_setopt(curl, CURLOPT_BUFFERSIZE, (long)config->recvpersecond);
        else
          my_setopt(curl, CURLOPT_BUFFERSIZE, (long)BUFFER_SIZE);

        my_setopt_str(curl, CURLOPT_URL, per->this_url);
        my_setopt(curl, CURLOPT_NOPROGRESS, global->noprogress?1L:0L);
        if(config->no_body)
          my_setopt(curl, CURLOPT_NOBODY, 1L);

        if(config->oauth_bearer)
          my_setopt_str(curl, CURLOPT_XOAUTH2_BEARER, config->oauth_bearer);

        my_setopt_str(curl, CURLOPT_PROXY, config->proxy);

        if(config->proxy && result) {
          errorf(global, "proxy support is disabled in this libcurl\n");
          config->synthetic_error = TRUE;
          result = CURLE_NOT_BUILT_IN;
          break;
        }

        /* new in libcurl 7.5 */
        if(config->proxy)
          my_setopt_enum(curl, CURLOPT_PROXYTYPE, config->proxyver);

        my_setopt_str(curl, CURLOPT_PROXYUSERPWD, config->proxyuserpwd);

        /* new in libcurl 7.3 */
        my_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, config->proxytunnel?1L:0L);

        /* new in libcurl 7.52.0 */
        if(config->preproxy)
          my_setopt_str(curl, CURLOPT_PRE_PROXY, config->preproxy);

        /* new in libcurl 7.10.6 */
        if(config->proxyanyauth)
          my_setopt_bitmask(curl, CURLOPT_PROXYAUTH,
                            (long)CURLAUTH_ANY);
        else if(config->proxynegotiate)
          my_setopt_bitmask(curl, CURLOPT_PROXYAUTH,
                            (long)CURLAUTH_GSSNEGOTIATE);
        else if(config->proxyntlm)
          my_setopt_bitmask(curl, CURLOPT_PROXYAUTH,
                            (long)CURLAUTH_NTLM);
        else if(config->proxydigest)
          my_setopt_bitmask(curl, CURLOPT_PROXYAUTH,
                            (long)CURLAUTH_DIGEST);
        else if(config->proxybasic)
          my_setopt_bitmask(curl, CURLOPT_PROXYAUTH,
                            (long)CURLAUTH_BASIC);

        /* new in libcurl 7.19.4 */
        my_setopt_str(curl, CURLOPT_NOPROXY, config->noproxy);

        my_setopt(curl, CURLOPT_SUPPRESS_CONNECT_HEADERS,
                  config->suppress_connect_headers?1L:0L);

        my_setopt(curl, CURLOPT_FAILONERROR, config->failonerror?1L:0L);
        my_setopt(curl, CURLOPT_REQUEST_TARGET, config->request_target);
        my_setopt(curl, CURLOPT_UPLOAD, per->uploadfile?1L:0L);
        my_setopt(curl, CURLOPT_DIRLISTONLY, config->dirlistonly?1L:0L);
        my_setopt(curl, CURLOPT_APPEND, config->ftp_append?1L:0L);

        if(config->netrc_opt)
          my_setopt_enum(curl, CURLOPT_NETRC, (long)CURL_NETRC_OPTIONAL);
        else if(config->netrc || config->netrc_file)
          my_setopt_enum(curl, CURLOPT_NETRC, (long)CURL_NETRC_REQUIRED);
        else
          my_setopt_enum(curl, CURLOPT_NETRC, (long)CURL_NETRC_IGNORED);

        if(config->netrc_file)
          my_setopt_str(curl, CURLOPT_NETRC_FILE, config->netrc_file);

        my_setopt(curl, CURLOPT_TRANSFERTEXT, config->use_ascii?1L:0L);
        if(config->login_options)
          my_setopt_str(curl, CURLOPT_LOGIN_OPTIONS, config->login_options);
        my_setopt_str(curl, CURLOPT_USERPWD, config->userpwd);
        my_setopt_str(curl, CURLOPT_RANGE, config->range);
        my_setopt(curl, CURLOPT_ERRORBUFFER, per->errorbuffer);
        my_setopt(curl, CURLOPT_TIMEOUT_MS, (long)(config->timeout * 1000));

        switch(config->httpreq) {
        case HTTPREQ_SIMPLEPOST:
          my_setopt_str(curl, CURLOPT_POSTFIELDS,
                        config->postfields);
          my_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                    config->postfieldsize);
          break;
        case HTTPREQ_MIMEPOST:
          /* free previous remainders */
          curl_mime_free(config->mimepost);
          config->mimepost = NULL;
          result = tool2curlmime(curl, config->mimeroot, &config->mimepost);
          if(result)
            break;
          my_setopt_mimepost(curl, CURLOPT_MIMEPOST, config->mimepost);
          break;
        default:
          break;
        }
        if(result)
          break;

        /* new in libcurl 7.81.0 */
        if(config->mime_options)
          my_setopt(curl, CURLOPT_MIME_OPTIONS, config->mime_options);

        /* new in libcurl 7.10.6 (default is Basic) */
        if(config->authtype)
          my_setopt_bitmask(curl, CURLOPT_HTTPAUTH, (long)config->authtype);

        my_setopt_slist(curl, CURLOPT_HTTPHEADER, config->headers);

        if(built_in_protos & (CURLPROTO_HTTP | CURLPROTO_RTSP)) {
          my_setopt_str(curl, CURLOPT_REFERER, config->referer);
          my_setopt_str(curl, CURLOPT_USERAGENT, config->useragent);
        }

        if(built_in_protos & CURLPROTO_HTTP) {

          long postRedir = 0;

          my_setopt(curl, CURLOPT_FOLLOWLOCATION,
                    config->followlocation?1L:0L);
          my_setopt(curl, CURLOPT_UNRESTRICTED_AUTH,
                    config->unrestricted_auth?1L:0L);

          my_setopt(curl, CURLOPT_AUTOREFERER, config->autoreferer?1L:0L);

          /* new in libcurl 7.36.0 */
          if(config->proxyheaders) {
            my_setopt_slist(curl, CURLOPT_PROXYHEADER, config->proxyheaders);
            my_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_SEPARATE);
          }

          /* new in libcurl 7.5 */
          my_setopt(curl, CURLOPT_MAXREDIRS, config->maxredirs);

          if(config->httpversion)
            my_setopt_enum(curl, CURLOPT_HTTP_VERSION, config->httpversion);
          else if(curlinfo->features & CURL_VERSION_HTTP2) {
            my_setopt_enum(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
          }

          /* curl 7.19.1 (the 301 version existed in 7.18.2),
             303 was added in 7.26.0 */
          if(config->post301)
            postRedir |= CURL_REDIR_POST_301;
          if(config->post302)
            postRedir |= CURL_REDIR_POST_302;
          if(config->post303)
            postRedir |= CURL_REDIR_POST_303;
          my_setopt(curl, CURLOPT_POSTREDIR, postRedir);

          /* new in libcurl 7.21.6 */
          if(config->encoding)
            my_setopt_str(curl, CURLOPT_ACCEPT_ENCODING, "");

          /* new in libcurl 7.21.6 */
          if(config->tr_encoding)
            my_setopt(curl, CURLOPT_TRANSFER_ENCODING, 1L);
          /* new in libcurl 7.64.0 */
          my_setopt(curl, CURLOPT_HTTP09_ALLOWED,
                    config->http09_allowed ? 1L : 0L);
          if(result) {
            errorf(global, "HTTP/0.9 is not supported in this build!\n");
            return result;
          }

        } /* (built_in_protos & CURLPROTO_HTTP) */

        my_setopt_str(curl, CURLOPT_FTPPORT, config->ftpport);
        my_setopt(curl, CURLOPT_LOW_SPEED_LIMIT,
                  config->low_speed_limit);
        my_setopt(curl, CURLOPT_LOW_SPEED_TIME, config->low_speed_time);
        my_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE,
                  config->sendpersecond);
        my_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE,
                  config->recvpersecond);

        if(config->use_resume)
          my_setopt(curl, CURLOPT_RESUME_FROM_LARGE, config->resume_from);
        else
          my_setopt(curl, CURLOPT_RESUME_FROM_LARGE, CURL_OFF_T_C(0));

        my_setopt_str(curl, CURLOPT_KEYPASSWD, config->key_passwd);
        my_setopt_str(curl, CURLOPT_PROXY_KEYPASSWD, config->proxy_key_passwd);

        if(use_proto & (CURLPROTO_SCP|CURLPROTO_SFTP)) {

          /* SSH and SSL private key uses same command-line option */
          /* new in libcurl 7.16.1 */
          my_setopt_str(curl, CURLOPT_SSH_PRIVATE_KEYFILE, config->key);
          /* new in libcurl 7.16.1 */
          my_setopt_str(curl, CURLOPT_SSH_PUBLIC_KEYFILE, config->pubkey);

          /* new in libcurl 7.17.1: SSH host key md5 checking allows us
             to fail if we are not talking to who we think we should */
          my_setopt_str(curl, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5,
                        config->hostpubmd5);

          /* new in libcurl 7.80.0: SSH host key sha256 checking allows us
             to fail if we are not talking to who we think we should */
          my_setopt_str(curl, CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256,
              config->hostpubsha256);

          /* new in libcurl 7.56.0 */
          if(config->ssh_compression)
            my_setopt(curl, CURLOPT_SSH_COMPRESSION, 1L);
        }

        if(config->cacert)
          my_setopt_str(curl, CURLOPT_CAINFO, config->cacert);
        if(config->proxy_cacert)
          my_setopt_str(curl, CURLOPT_PROXY_CAINFO, config->proxy_cacert);

        if(config->capath) {
          result = res_setopt_str(curl, CURLOPT_CAPATH, config->capath);
          if(result == CURLE_NOT_BUILT_IN) {
            warnf(global, "ignoring %s, not supported by libcurl\n",
                  capath_from_env?
                  "SSL_CERT_DIR environment variable":"--capath");
          }
          else if(result)
            break;
        }
        /* For the time being if --proxy-capath is not set then we use the
           --capath value for it, if any. See #1257 */
        if(config->proxy_capath || config->capath) {
          result = res_setopt_str(curl, CURLOPT_PROXY_CAPATH,
                                  (config->proxy_capath ?
                                   config->proxy_capath :
                                   config->capath));
          if(result == CURLE_NOT_BUILT_IN) {
            if(config->proxy_capath) {
              warnf(global,
                    "ignoring --proxy-capath, not supported by libcurl\n");
            }
          }
          else if(result)
            break;
        }

        if(config->crlfile)
          my_setopt_str(curl, CURLOPT_CRLFILE, config->crlfile);
        if(config->proxy_crlfile)
          my_setopt_str(curl, CURLOPT_PROXY_CRLFILE, config->proxy_crlfile);
        else if(config->crlfile) /* CURLOPT_PROXY_CRLFILE default is crlfile */
          my_setopt_str(curl, CURLOPT_PROXY_CRLFILE, config->crlfile);

        if(config->pinnedpubkey)
          my_setopt_str(curl, CURLOPT_PINNEDPUBLICKEY, config->pinnedpubkey);

        if(config->ssl_ec_curves)
          my_setopt_str(curl, CURLOPT_SSL_EC_CURVES, config->ssl_ec_curves);

        if(curlinfo->features & CURL_VERSION_SSL) {
          /* Check if config->cert is a PKCS#11 URI and set the
           * config->cert_type if necessary */
          if(config->cert) {
            if(!config->cert_type) {
              if(is_pkcs11_uri(config->cert)) {
                config->cert_type = strdup("ENG");
              }
            }
          }

          /* Check if config->key is a PKCS#11 URI and set the
           * config->key_type if necessary */
          if(config->key) {
            if(!config->key_type) {
              if(is_pkcs11_uri(config->key)) {
                config->key_type = strdup("ENG");
              }
            }
          }

          /* Check if config->proxy_cert is a PKCS#11 URI and set the
           * config->proxy_type if necessary */
          if(config->proxy_cert) {
            if(!config->proxy_cert_type) {
              if(is_pkcs11_uri(config->proxy_cert)) {
                config->proxy_cert_type = strdup("ENG");
              }
            }
          }

          /* Check if config->proxy_key is a PKCS#11 URI and set the
           * config->proxy_key_type if necessary */
          if(config->proxy_key) {
            if(!config->proxy_key_type) {
              if(is_pkcs11_uri(config->proxy_key)) {
                config->proxy_key_type = strdup("ENG");
              }
            }
          }

          /* In debug build of curl tool, using
           *    --cert loadmem=<filename>:<password> --cert-type p12
           *  must do the same thing as classic:
           *    --cert <filename>:<password> --cert-type p12
           *  but is designed to test blob */
#if defined(CURLDEBUG) || defined(DEBUGBUILD)
          if(config->cert && (strlen(config->cert) > 8) &&
             (memcmp(config->cert, "loadmem=",8) == 0)) {
            FILE *fInCert = fopen(config->cert + 8, "rb");
            void *certdata = NULL;
            long filesize = 0;
            bool continue_reading = fInCert != NULL;
            if(continue_reading)
              continue_reading = fseek(fInCert, 0, SEEK_END) == 0;
            if(continue_reading)
              filesize = ftell(fInCert);
            if(filesize < 0)
              continue_reading = FALSE;
            if(continue_reading)
              continue_reading = fseek(fInCert, 0, SEEK_SET) == 0;
            if(continue_reading)
              certdata = malloc(((size_t)filesize) + 1);
            if((!certdata) ||
                ((int)fread(certdata, (size_t)filesize, 1, fInCert) != 1))
              continue_reading = FALSE;
            if(fInCert)
              fclose(fInCert);
            if((filesize > 0) && continue_reading) {
              struct curl_blob structblob;
              structblob.data = certdata;
              structblob.len = (size_t)filesize;
              structblob.flags = CURL_BLOB_COPY;
              my_setopt_str(curl, CURLOPT_SSLCERT_BLOB, &structblob);
              /* if test run well, we are sure we don't reuse
               * original mem pointer */
              memset(certdata, 0, (size_t)filesize);
            }
            free(certdata);
          }
          else
#endif
          my_setopt_str(curl, CURLOPT_SSLCERT, config->cert);
          my_setopt_str(curl, CURLOPT_PROXY_SSLCERT, config->proxy_cert);
          my_setopt_str(curl, CURLOPT_SSLCERTTYPE, config->cert_type);
          my_setopt_str(curl, CURLOPT_PROXY_SSLCERTTYPE,
                        config->proxy_cert_type);


#if defined(CURLDEBUG) || defined(DEBUGBUILD)
          if(config->key && (strlen(config->key) > 8) &&
             (memcmp(config->key, "loadmem=",8) == 0)) {
            FILE *fInCert = fopen(config->key + 8, "rb");
            void *certdata = NULL;
            long filesize = 0;
            bool continue_reading = fInCert != NULL;
            if(continue_reading)
              continue_reading = fseek(fInCert, 0, SEEK_END) == 0;
            if(continue_reading)
              filesize = ftell(fInCert);
            if(filesize < 0)
              continue_reading = FALSE;
            if(continue_reading)
              continue_reading = fseek(fInCert, 0, SEEK_SET) == 0;
            if(continue_reading)
              certdata = malloc(((size_t)filesize) + 1);
            if((!certdata) ||
                ((int)fread(certdata, (size_t)filesize, 1, fInCert) != 1))
              continue_reading = FALSE;
            if(fInCert)
              fclose(fInCert);
            if((filesize > 0) && continue_reading) {
              struct curl_blob structblob;
              structblob.data = certdata;
              structblob.len = (size_t)filesize;
              structblob.flags = CURL_BLOB_COPY;
              my_setopt_str(curl, CURLOPT_SSLKEY_BLOB, &structblob);
              /* if test run well, we are sure we don't reuse
               * original mem pointer */
              memset(certdata, 0, (size_t)filesize);
            }
            free(certdata);
          }
          else
#endif
          my_setopt_str(curl, CURLOPT_SSLKEY, config->key);
          my_setopt_str(curl, CURLOPT_PROXY_SSLKEY, config->proxy_key);
          my_setopt_str(curl, CURLOPT_SSLKEYTYPE, config->key_type);
          my_setopt_str(curl, CURLOPT_PROXY_SSLKEYTYPE,
                        config->proxy_key_type);
          my_setopt_str(curl, CURLOPT_AWS_SIGV4,
                        config->aws_sigv4);

          if(config->insecure_ok) {
            my_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            my_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
          }
          else {
            my_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            /* libcurl default is strict verifyhost -> 2L   */
            /* my_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L); */
          }

          if(config->doh_insecure_ok) {
            my_setopt(curl, CURLOPT_DOH_SSL_VERIFYPEER, 0L);
            my_setopt(curl, CURLOPT_DOH_SSL_VERIFYHOST, 0L);
          }

          if(config->proxy_insecure_ok) {
            my_setopt(curl, CURLOPT_PROXY_SSL_VERIFYPEER, 0L);
            my_setopt(curl, CURLOPT_PROXY_SSL_VERIFYHOST, 0L);
          }
          else {
            my_setopt(curl, CURLOPT_PROXY_SSL_VERIFYPEER, 1L);
          }

          if(config->verifystatus)
            my_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 1L);

          if(config->doh_verifystatus)
            my_setopt(curl, CURLOPT_DOH_SSL_VERIFYSTATUS, 1L);

          if(config->falsestart)
            my_setopt(curl, CURLOPT_SSL_FALSESTART, 1L);

          my_setopt_enum(curl, CURLOPT_SSLVERSION,
                         config->ssl_version | config->ssl_version_max);
          if(config->proxy)
            my_setopt_enum(curl, CURLOPT_PROXY_SSLVERSION,
                           config->proxy_ssl_version);

          {
            long mask =
              (config->ssl_allow_beast ?
               CURLSSLOPT_ALLOW_BEAST : 0) |
              (config->ssl_no_revoke ?
               CURLSSLOPT_NO_REVOKE : 0) |
              (config->ssl_revoke_best_effort ?
               CURLSSLOPT_REVOKE_BEST_EFFORT : 0) |
              (config->native_ca_store ?
               CURLSSLOPT_NATIVE_CA : 0) |
              (config->ssl_auto_client_cert ?
               CURLSSLOPT_AUTO_CLIENT_CERT : 0);

            if(mask)
              my_setopt_bitmask(curl, CURLOPT_SSL_OPTIONS, mask);
          }

          {
            long mask =
              (config->proxy_ssl_allow_beast ?
               CURLSSLOPT_ALLOW_BEAST : 0) |
              (config->proxy_ssl_auto_client_cert ?
               CURLSSLOPT_AUTO_CLIENT_CERT : 0);

            if(mask)
              my_setopt_bitmask(curl, CURLOPT_PROXY_SSL_OPTIONS, mask);
          }
        }

        if(config->path_as_is)
          my_setopt(curl, CURLOPT_PATH_AS_IS, 1L);

        if((use_proto & (CURLPROTO_SCP|CURLPROTO_SFTP)) &&
           !config->insecure_ok) {
          char *known = findfile(".ssh/known_hosts", FALSE);
          if(known) {
            /* new in curl 7.19.6 */
            result = res_setopt_str(curl, CURLOPT_SSH_KNOWNHOSTS, known);
            curl_free(known);
            if(result == CURLE_UNKNOWN_OPTION)
              /* libssh2 version older than 1.1.1 */
              result = CURLE_OK;
            if(result)
              break;
          }
          else
            warnf(global, "Couldn't find a known_hosts file!");
        }

        if(config->no_body || config->remote_time) {
          /* no body or use remote time */
          my_setopt(curl, CURLOPT_FILETIME, 1L);
        }

        my_setopt(curl, CURLOPT_CRLF, config->crlf?1L:0L);
        my_setopt_slist(curl, CURLOPT_QUOTE, config->quote);
        my_setopt_slist(curl, CURLOPT_POSTQUOTE, config->postquote);
        my_setopt_slist(curl, CURLOPT_PREQUOTE, config->prequote);

        if(config->cookies) {
          struct curlx_dynbuf cookies;
          struct curl_slist *cl;

          /* The maximum size needs to match MAX_NAME in cookie.h */
          curlx_dyn_init(&cookies, 4096);
          for(cl = config->cookies; cl; cl = cl->next) {
            if(cl == config->cookies)
              result = curlx_dyn_addf(&cookies, "%s", cl->data);
            else
              result = curlx_dyn_addf(&cookies, ";%s", cl->data);

            if(result)
              break;
          }

          my_setopt_str(curl, CURLOPT_COOKIE, curlx_dyn_ptr(&cookies));
          curlx_dyn_free(&cookies);
        }

        if(config->cookiefiles) {
          struct curl_slist *cfl;

          for(cfl = config->cookiefiles; cfl; cfl = cfl->next)
            my_setopt_str(curl, CURLOPT_COOKIEFILE, cfl->data);
        }

        /* new in libcurl 7.9 */
        if(config->cookiejar)
          my_setopt_str(curl, CURLOPT_COOKIEJAR, config->cookiejar);

        /* new in libcurl 7.9.7 */
        my_setopt(curl, CURLOPT_COOKIESESSION, config->cookiesession?1L:0L);

        my_setopt_enum(curl, CURLOPT_TIMECONDITION, (long)config->timecond);
        my_setopt(curl, CURLOPT_TIMEVALUE_LARGE, config->condtime);
        my_setopt_str(curl, CURLOPT_CUSTOMREQUEST, config->customrequest);
        customrequest_helper(config, config->httpreq, config->customrequest);
        my_setopt(curl, CURLOPT_STDERR, global->errors);

        /* three new ones in libcurl 7.3: */
        my_setopt_str(curl, CURLOPT_INTERFACE, config->iface);
        my_setopt_str(curl, CURLOPT_KRBLEVEL, config->krblevel);
        progressbarinit(&per->progressbar, config);

        if((global->progressmode == CURL_PROGRESS_BAR) &&
           !global->noprogress && !global->mute) {
          /* we want the alternative style, then we have to implement it
             ourselves! */
          my_setopt(curl, CURLOPT_XFERINFOFUNCTION, tool_progress_cb);
          my_setopt(curl, CURLOPT_XFERINFODATA, per);
        }
        else if(per->uploadfile && !strcmp(per->uploadfile, ".")) {
          /* when reading from stdin in non-blocking mode, we use the progress
             function to unpause a busy read */
          my_setopt(curl, CURLOPT_NOPROGRESS, 0L);
          my_setopt(curl, CURLOPT_XFERINFOFUNCTION, tool_readbusy_cb);
          my_setopt(curl, CURLOPT_XFERINFODATA, per);
        }

        /* new in libcurl 7.24.0: */
        if(config->dns_servers)
          my_setopt_str(curl, CURLOPT_DNS_SERVERS, config->dns_servers);

        /* new in libcurl 7.33.0: */
        if(config->dns_interface)
          my_setopt_str(curl, CURLOPT_DNS_INTERFACE, config->dns_interface);
        if(config->dns_ipv4_addr)
          my_setopt_str(curl, CURLOPT_DNS_LOCAL_IP4, config->dns_ipv4_addr);
        if(config->dns_ipv6_addr)
        my_setopt_str(curl, CURLOPT_DNS_LOCAL_IP6, config->dns_ipv6_addr);

        /* new in libcurl 7.6.2: */
        my_setopt_slist(curl, CURLOPT_TELNETOPTIONS, config->telnet_options);

        /* new in libcurl 7.7: */
        my_setopt_str(curl, CURLOPT_RANDOM_FILE, config->random_file);
        my_setopt_str(curl, CURLOPT_EGDSOCKET, config->egd_file);
        my_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,
                  (long)(config->connecttimeout * 1000));

        if(config->doh_url)
          my_setopt_str(curl, CURLOPT_DOH_URL, config->doh_url);

        if(config->cipher_list)
          my_setopt_str(curl, CURLOPT_SSL_CIPHER_LIST, config->cipher_list);

        if(config->proxy_cipher_list)
          my_setopt_str(curl, CURLOPT_PROXY_SSL_CIPHER_LIST,
                        config->proxy_cipher_list);

        if(config->cipher13_list)
          my_setopt_str(curl, CURLOPT_TLS13_CIPHERS, config->cipher13_list);

        if(config->proxy_cipher13_list)
          my_setopt_str(curl, CURLOPT_PROXY_TLS13_CIPHERS,
                        config->proxy_cipher13_list);

        /* new in libcurl 7.9.2: */
        if(config->disable_epsv)
          /* disable it */
          my_setopt(curl, CURLOPT_FTP_USE_EPSV, 0L);

        /* new in libcurl 7.10.5 */
        if(config->disable_eprt)
          /* disable it */
          my_setopt(curl, CURLOPT_FTP_USE_EPRT, 0L);

        if(global->tracetype != TRACE_NONE) {
          my_setopt(curl, CURLOPT_DEBUGFUNCTION, tool_debug_cb);
          my_setopt(curl, CURLOPT_DEBUGDATA, config);
          my_setopt(curl, CURLOPT_VERBOSE, 1L);
        }

        /* new in curl 7.9.3 */
        if(config->engine) {
          result = res_setopt_str(curl, CURLOPT_SSLENGINE, config->engine);
          if(result)
            break;
        }

        /* new in curl 7.10.7, extended in 7.19.4. Modified to use
           CREATE_DIR_RETRY in 7.49.0 */
        my_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS,
                  (long)(config->ftp_create_dirs?
                         CURLFTP_CREATE_DIR_RETRY:
                         CURLFTP_CREATE_DIR_NONE));

        /* new in curl 7.10.8 */
        if(config->max_filesize)
          my_setopt(curl, CURLOPT_MAXFILESIZE_LARGE,
                    config->max_filesize);

        my_setopt(curl, CURLOPT_IPRESOLVE, config->ip_version);

        /* new in curl 7.15.5 */
        if(config->ftp_ssl_reqd)
          my_setopt_enum(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        /* new in curl 7.11.0 */
        else if(config->ftp_ssl)
          my_setopt_enum(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_TRY);

        /* new in curl 7.16.0 */
        else if(config->ftp_ssl_control)
          my_setopt_enum(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_CONTROL);

        /* new in curl 7.16.1 */
        if(config->ftp_ssl_ccc)
          my_setopt_enum(curl, CURLOPT_FTP_SSL_CCC,
                         (long)config->ftp_ssl_ccc_mode);

        /* new in curl 7.19.4 */
        if(config->socks5_gssapi_nec)
          my_setopt_str(curl, CURLOPT_SOCKS5_GSSAPI_NEC,
                        config->socks5_gssapi_nec);

        /* new in curl 7.55.0 */
        if(config->socks5_auth)
          my_setopt_bitmask(curl, CURLOPT_SOCKS5_AUTH,
                            (long)config->socks5_auth);

        /* new in curl 7.43.0 */
        if(config->proxy_service_name)
          my_setopt_str(curl, CURLOPT_PROXY_SERVICE_NAME,
                        config->proxy_service_name);

        /* new in curl 7.43.0 */
        if(config->service_name)
          my_setopt_str(curl, CURLOPT_SERVICE_NAME,
                        config->service_name);

        /* curl 7.13.0 */
        my_setopt_str(curl, CURLOPT_FTP_ACCOUNT, config->ftp_account);
        my_setopt(curl, CURLOPT_IGNORE_CONTENT_LENGTH, config->ignorecl?1L:0L);

        /* curl 7.14.2 */
        my_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, config->ftp_skip_ip?1L:0L);

        /* curl 7.15.1 */
        my_setopt(curl, CURLOPT_FTP_FILEMETHOD, (long)config->ftp_filemethod);

        /* curl 7.15.2 */
        if(config->localport) {
          my_setopt(curl, CURLOPT_LOCALPORT, config->localport);
          my_setopt_str(curl, CURLOPT_LOCALPORTRANGE, config->localportrange);
        }

        /* curl 7.15.5 */
        my_setopt_str(curl, CURLOPT_FTP_ALTERNATIVE_TO_USER,
                      config->ftp_alternative_to_user);

        /* curl 7.16.0 */
        if(config->disable_sessionid)
          /* disable it */
          my_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);

        /* curl 7.16.2 */
        if(config->raw) {
          my_setopt(curl, CURLOPT_HTTP_CONTENT_DECODING, 0L);
          my_setopt(curl, CURLOPT_HTTP_TRANSFER_DECODING, 0L);
        }

        /* curl 7.17.1 */
        if(!config->nokeepalive) {
          my_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
          if(config->alivetime) {
            my_setopt(curl, CURLOPT_TCP_KEEPIDLE, config->alivetime);
            my_setopt(curl, CURLOPT_TCP_KEEPINTVL, config->alivetime);
          }
        }
        else
          my_setopt(curl, CURLOPT_TCP_KEEPALIVE, 0L);

        /* curl 7.20.0 */
        if(config->tftp_blksize)
          my_setopt(curl, CURLOPT_TFTP_BLKSIZE, config->tftp_blksize);

        if(config->mail_from)
          my_setopt_str(curl, CURLOPT_MAIL_FROM, config->mail_from);

        if(config->mail_rcpt)
          my_setopt_slist(curl, CURLOPT_MAIL_RCPT, config->mail_rcpt);

        /* curl 7.69.x */
        my_setopt(curl, CURLOPT_MAIL_RCPT_ALLLOWFAILS,
          config->mail_rcpt_allowfails ? 1L : 0L);

        /* curl 7.20.x */
        if(config->ftp_pret)
          my_setopt(curl, CURLOPT_FTP_USE_PRET, 1L);

        if(config->create_file_mode)
          my_setopt(curl, CURLOPT_NEW_FILE_PERMS, config->create_file_mode);

        if(config->proto_present)
          my_setopt_flags(curl, CURLOPT_PROTOCOLS, config->proto);
        if(config->proto_redir_present)
          my_setopt_flags(curl, CURLOPT_REDIR_PROTOCOLS, config->proto_redir);

        if(config->content_disposition
           && (urlnode->flags & GETOUT_USEREMOTE))
          hdrcbdata->honor_cd_filename = TRUE;
        else
          hdrcbdata->honor_cd_filename = FALSE;

        hdrcbdata->outs = outs;
        hdrcbdata->heads = heads;
        hdrcbdata->etag_save = etag_save;
        hdrcbdata->global = global;
        hdrcbdata->config = config;

        my_setopt(curl, CURLOPT_HEADERFUNCTION, tool_header_cb);
        my_setopt(curl, CURLOPT_HEADERDATA, per);

        if(config->resolve)
          /* new in 7.21.3 */
          my_setopt_slist(curl, CURLOPT_RESOLVE, config->resolve);

        if(config->connect_to)
          /* new in 7.49.0 */
          my_setopt_slist(curl, CURLOPT_CONNECT_TO, config->connect_to);

        /* new in 7.21.4 */
        if(curlinfo->features & CURL_VERSION_TLSAUTH_SRP) {
          if(config->tls_username)
            my_setopt_str(curl, CURLOPT_TLSAUTH_USERNAME,
                          config->tls_username);
          if(config->tls_password)
            my_setopt_str(curl, CURLOPT_TLSAUTH_PASSWORD,
                          config->tls_password);
          if(config->tls_authtype)
            my_setopt_str(curl, CURLOPT_TLSAUTH_TYPE,
                          config->tls_authtype);
          if(config->proxy_tls_username)
            my_setopt_str(curl, CURLOPT_PROXY_TLSAUTH_USERNAME,
                          config->proxy_tls_username);
          if(config->proxy_tls_password)
            my_setopt_str(curl, CURLOPT_PROXY_TLSAUTH_PASSWORD,
                          config->proxy_tls_password);
          if(config->proxy_tls_authtype)
            my_setopt_str(curl, CURLOPT_PROXY_TLSAUTH_TYPE,
                          config->proxy_tls_authtype);
        }

        /* new in 7.22.0 */
        if(config->gssapi_delegation)
          my_setopt_str(curl, CURLOPT_GSSAPI_DELEGATION,
                        config->gssapi_delegation);

        if(config->mail_auth)
          my_setopt_str(curl, CURLOPT_MAIL_AUTH, config->mail_auth);

        /* new in 7.66.0 */
        if(config->sasl_authzid)
          my_setopt_str(curl, CURLOPT_SASL_AUTHZID, config->sasl_authzid);

        /* new in 7.31.0 */
        if(config->sasl_ir)
          my_setopt(curl, CURLOPT_SASL_IR, 1L);

        if(config->nonpn) {
          my_setopt(curl, CURLOPT_SSL_ENABLE_NPN, 0L);
        }

        if(config->noalpn) {
          my_setopt(curl, CURLOPT_SSL_ENABLE_ALPN, 0L);
        }

        /* new in 7.40.0, abstract support added in 7.53.0 */
        if(config->unix_socket_path) {
          if(config->abstract_unix_socket) {
            my_setopt_str(curl, CURLOPT_ABSTRACT_UNIX_SOCKET,
                          config->unix_socket_path);
          }
          else {
            my_setopt_str(curl, CURLOPT_UNIX_SOCKET_PATH,
                          config->unix_socket_path);
          }
        }

        /* new in 7.45.0 */
        if(config->proto_default)
          my_setopt_str(curl, CURLOPT_DEFAULT_PROTOCOL, config->proto_default);

        /* new in 7.47.0 */
        if(config->expect100timeout > 0)
          my_setopt_str(curl, CURLOPT_EXPECT_100_TIMEOUT_MS,
                        (long)(config->expect100timeout*1000));

        /* new in 7.48.0 */
        if(config->tftp_no_options)
          my_setopt(curl, CURLOPT_TFTP_NO_OPTIONS, 1L);

        /* new in 7.59.0 */
        if(config->happy_eyeballs_timeout_ms != CURL_HET_DEFAULT)
          my_setopt(curl, CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS,
                    config->happy_eyeballs_timeout_ms);

        /* new in 7.60.0 */
        if(config->haproxy_protocol)
          my_setopt(curl, CURLOPT_HAPROXYPROTOCOL, 1L);

        if(config->disallow_username_in_url)
          my_setopt(curl, CURLOPT_DISALLOW_USERNAME_IN_URL, 1L);

        if(config->altsvc)
          my_setopt_str(curl, CURLOPT_ALTSVC, config->altsvc);

        if(config->hsts)
          my_setopt_str(curl, CURLOPT_HSTS, config->hsts);

        /* initialize retry vars for loop below */
        per->retry_sleep_default = (config->retry_delay) ?
          config->retry_delay*1000L : RETRY_SLEEP_DEFAULT; /* ms */
        per->retry_numretries = config->req_retry;
        per->retry_sleep = per->retry_sleep_default; /* ms */
        per->retrystart = tvnow();

        state->li++;
        /* Here's looping around each globbed URL */
        if(state->li >= urlnum) {
          state->li = 0;
          state->urlnum = 0; /* forced reglob of URLs */
          glob_cleanup(state->urls);
          state->urls = NULL;
          state->up++;
          Curl_safefree(state->uploadfile); /* clear it to get the next */
        }
      }
      else {
        /* Free this URL node data without destroying the
           node itself nor modifying next pointer. */
        Curl_safefree(urlnode->outfile);
        Curl_safefree(urlnode->infile);
        urlnode->flags = 0;
        glob_cleanup(state->urls);
        state->urls = NULL;
        state->urlnum = 0;

        Curl_safefree(state->outfiles);
        Curl_safefree(state->uploadfile);
        if(state->inglob) {
          /* Free list of globbed upload files */
          glob_cleanup(state->inglob);
          state->inglob = NULL;
        }
        config->state.urlnode = urlnode->next;
        state->up = 0;
        continue;
      }
    }
    break;
  }

  if(!*added || result) {
    *added = FALSE;
    single_transfer_cleanup(config);
  }
  return result;
}