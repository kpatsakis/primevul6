int TNEFParseMemory(BYTE *memory, long size, TNEFStruct *TNEF) {
  TNEFMemInfo minfo;

  DEBUG(TNEF->Debug, 1, "Attempting to parse memory block...\n");

  minfo.dataStart = memory;
  minfo.ptr = memory;
  minfo.size = size;
  minfo.Debug = TNEF->Debug;
  TNEF->IO.data = (void *)&minfo;
  TNEF->IO.InitProc = TNEFMemory_Open;
  TNEF->IO.ReadProc = TNEFMemory_Read;
  TNEF->IO.CloseProc = TNEFMemory_Close;
  return TNEFParse(TNEF);
}