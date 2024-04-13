int TNEFFile_Read(TNEFIOStruct *IO, int size, int count, void *dest) {
  TNEFFileInfo *finfo;
  finfo = (TNEFFileInfo *)IO->data;

  DEBUG2(finfo->Debug, 3, "Reading %i blocks of %i size", count, size);
  if (finfo->fptr != NULL) {
    return fread((BYTE *)dest, size, count, finfo->fptr);
  } else {
    return -1;
  }
}