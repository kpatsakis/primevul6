int TNEFParseFile(char *filename, TNEFStruct *TNEF) {
  TNEFFileInfo finfo;

  if (TNEF->Debug >= 1)
    printf("Attempting to parse %s...\n", filename);


  finfo.filename = filename;
  finfo.fptr = NULL;
  finfo.Debug = TNEF->Debug;
  TNEF->IO.data = (void *)&finfo;
  TNEF->IO.InitProc = TNEFFile_Open;
  TNEF->IO.ReadProc = TNEFFile_Read;
  TNEF->IO.CloseProc = TNEFFile_Close;
  return TNEFParse(TNEF);
}