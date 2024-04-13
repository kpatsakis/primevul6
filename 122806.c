int TNEFFile_Open(TNEFIOStruct *IO) {
  TNEFFileInfo *finfo;
  finfo = (TNEFFileInfo *)IO->data;

  DEBUG1(finfo->Debug, 3, "Opening %s", finfo->filename);
  if ((finfo->fptr = fopen(finfo->filename, "rb")) == NULL) {
    return -1;
  } else {
    return 0;
  }
}