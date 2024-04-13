InSliceInfo::InSliceInfo (PixelType tifb,
                          char * b,
                          PixelType tifl,
                          size_t xpst,
                          size_t ypst,
                          size_t spst,
                          int xsm, int ysm,
                          bool f, bool s,
                          double fv)
:
    typeInFrameBuffer (tifb),
    typeInFile (tifl),
    base(b),
    xPointerStride (xpst),
    yPointerStride (ypst),
    sampleStride (spst),
    xSampling (xsm),
    ySampling (ysm),
    fill (f),
    skip (s),
    fillValue (fv)
{
    // empty
}