CURLcode Curl_fopen(struct Curl_easy *data, const char *filename,
                    FILE **fh, char **tempname)
{
  CURLcode result = CURLE_WRITE_ERROR;
  unsigned char randsuffix[9];
  char *tempstore = NULL;
  struct_stat sb;
  int fd = -1;
  *tempname = NULL;

  if(stat(filename, &sb) == -1 || !S_ISREG(sb.st_mode)) {
    /* a non-regular file, fallback to direct fopen() */
    *fh = fopen(filename, FOPEN_WRITETEXT);
    if(*fh)
      return CURLE_OK;
    goto fail;
  }

  result = Curl_rand_hex(data, randsuffix, sizeof(randsuffix));
  if(result)
    goto fail;

  tempstore = aprintf("%s.%s.tmp", filename, randsuffix);
  if(!tempstore) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  result = CURLE_WRITE_ERROR;
  fd = open(tempstore, O_WRONLY | O_CREAT | O_EXCL, 0600);
  if(fd == -1)
    goto fail;

#ifdef HAVE_FCHMOD
  {
    struct_stat nsb;
    if((fstat(fd, &nsb) != -1) &&
       (nsb.st_uid == sb.st_uid) && (nsb.st_gid == sb.st_gid)) {
      /* if the user and group are the same, clone the original mode */
      if(fchmod(fd, sb.st_mode) == -1)
        goto fail;
    }
  }
#endif

  *fh = fdopen(fd, FOPEN_WRITETEXT);
  if(!*fh)
    goto fail;

  *tempname = tempstore;
  return CURLE_OK;

fail:
  if(fd != -1) {
    close(fd);
    unlink(tempstore);
  }

  free(tempstore);

  *tempname = NULL;
  return result;
}