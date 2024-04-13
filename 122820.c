int TNEFFile_Close(TNEFIOStruct *IO) {
  TNEFFileInfo *finfo;
  finfo = (TNEFFileInfo *)IO->data;

  DEBUG1(finfo->Debug, 3, "Closing file %s", finfo->filename);
  if (finfo->fptr != NULL) {
    fclose(finfo->fptr);
    finfo->fptr = NULL;
  }
  return 0;
}