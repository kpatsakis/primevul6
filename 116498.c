MagickExport QuantizeInfo *DestroyQuantizeInfo(QuantizeInfo *quantize_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(quantize_info != (QuantizeInfo *) NULL);
  assert(quantize_info->signature == MagickCoreSignature);
  quantize_info->signature=(~MagickCoreSignature);
  quantize_info=(QuantizeInfo *) RelinquishMagickMemory(quantize_info);
  return(quantize_info);
}