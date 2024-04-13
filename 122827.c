int TNEFMemory_Read(TNEFIOStruct *IO, int size, int count, void *dest) {
  TNEFMemInfo *minfo;
  int length;
  long max;
  minfo = (TNEFMemInfo *)IO->data;

  length = count * size;
  max = (minfo->dataStart + minfo->size) - (minfo->ptr);
  if (length > max) {
    return -1;
  }

  DEBUG1(minfo->Debug, 3, "Copying %i bytes", length);

  memcpy(dest, minfo->ptr, length);
  minfo->ptr += length;
  return count;
}