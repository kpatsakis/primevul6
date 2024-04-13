rfbMpiToBytes(const gcry_mpi_t value, uint8_t *result, size_t size)
{
  gcry_error_t error;
  size_t len;
  int i;

  error = gcry_mpi_print(GCRYMPI_FMT_USG, result, size, &len, value);
  if (gcry_err_code(error) != GPG_ERR_NO_ERROR)
  {
    rfbClientLog("gcry_mpi_print error: %s\n", gcry_strerror(error));
    return FALSE;
  }
  for (i=size-1;i>(int)size-1-(int)len;--i)
    result[i] = result[i-size+len];
  for (;i>=0;--i)
    result[i] = 0;
  return TRUE;
}