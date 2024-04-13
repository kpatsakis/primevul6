CURLcode operate(struct GlobalConfig *global, int argc, argv_item_t argv[])
{
  CURLcode result = CURLE_OK;
  char *first_arg = argc > 1 ? curlx_convert_tchar_to_UTF8(argv[1]) : NULL;

  /* Setup proper locale from environment */
#ifdef HAVE_SETLOCALE
  setlocale(LC_ALL, "");
#endif

  /* Parse .curlrc if necessary */
  if((argc == 1) ||
     (first_arg && strncmp(first_arg, "-q", 2) &&
      !curl_strequal(first_arg, "--disable"))) {
    parseconfig(NULL, global); /* ignore possible failure */

    /* If we had no arguments then make sure a url was specified in .curlrc */
    if((argc < 2) && (!global->first->url_list)) {
      helpf(global->errors, NULL);
      result = CURLE_FAILED_INIT;
    }
  }

  curlx_unicodefree(first_arg);

  if(!result) {
    /* Parse the command line arguments */
    ParameterError res = parse_args(global, argc, argv);
    if(res) {
      result = CURLE_OK;

      /* Check if we were asked for the help */
      if(res == PARAM_HELP_REQUESTED)
        tool_help(global->help_category);
      /* Check if we were asked for the manual */
      else if(res == PARAM_MANUAL_REQUESTED)
        hugehelp();
      /* Check if we were asked for the version information */
      else if(res == PARAM_VERSION_INFO_REQUESTED)
        tool_version_info();
      /* Check if we were asked to list the SSL engines */
      else if(res == PARAM_ENGINES_REQUESTED)
        tool_list_engines();
      else if(res == PARAM_LIBCURL_UNSUPPORTED_PROTOCOL)
        result = CURLE_UNSUPPORTED_PROTOCOL;
      else if(res == PARAM_READ_ERROR)
        result = CURLE_READ_ERROR;
      else
        result = CURLE_FAILED_INIT;
    }
    else {
#ifndef CURL_DISABLE_LIBCURL_OPTION
      if(global->libcurl) {
        /* Initialise the libcurl source output */
        result = easysrc_init();
      }
#endif

      /* Perform the main operations */
      if(!result) {
        size_t count = 0;
        struct OperationConfig *operation = global->first;
        CURLSH *share = curl_share_init();
        if(!share) {
#ifndef CURL_DISABLE_LIBCURL_OPTION
          if(global->libcurl) {
            /* Cleanup the libcurl source output */
            easysrc_cleanup();
          }
#endif
          return CURLE_OUT_OF_MEMORY;
        }

        curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
        curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
        curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_PSL);

        /* Get the required arguments for each operation */
        do {
          result = get_args(operation, count++);

          operation = operation->next;
        } while(!result && operation);

        /* Set the current operation pointer */
        global->current = global->first;

        /* now run! */
        result = run_all_transfers(global, share, result);

        curl_share_cleanup(share);
#ifndef CURL_DISABLE_LIBCURL_OPTION
        if(global->libcurl) {
          /* Cleanup the libcurl source output */
          easysrc_cleanup();

          /* Dump the libcurl code if previously enabled */
          dumpeasysrc(global);
        }
#endif
      }
      else
        errorf(global, "out of memory\n");
    }
  }

  return result;
}