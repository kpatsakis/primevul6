ModuleExport void UnregisterEMFImage(void)
{
  (void) UnregisterMagickInfo("EMF");
  (void) UnregisterMagickInfo("WMF");
}