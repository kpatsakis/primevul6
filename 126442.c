ModuleExport void UnregisterPICTImage(void)
{
  (void) UnregisterMagickInfo("PCT");
  (void) UnregisterMagickInfo("PICT");
}