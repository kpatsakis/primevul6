MagickExport void *GetVirtualMemoryBlob(const MemoryInfo *memory_info)
{
  assert(memory_info != (const MemoryInfo *) NULL);
  assert(memory_info->signature == MagickCoreSignature);
  return(memory_info->blob);
}