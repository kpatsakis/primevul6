ModuleExport void UnregisterTXTImage(void)
{
  (void) UnregisterMagickInfo("SPARSE-COLOR");
  (void) UnregisterMagickInfo("TEXT");
  (void) UnregisterMagickInfo("TXT");
}