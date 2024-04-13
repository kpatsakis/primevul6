int TNEFMemory_Open(TNEFIOStruct *IO) {
  TNEFMemInfo *minfo;
  minfo = (TNEFMemInfo *)IO->data;

  minfo->ptr = minfo->dataStart;
  return 0;
}