generatePreview (const char inFileName[],
		 float exposure,
		 int previewWidth,
		 int &previewHeight,
		 Array2D <PreviewRgba> &previewPixels)
{
    //
    // Read the input file
    //

    RgbaInputFile in (inFileName);

    Box2i dw = in.dataWindow();
    float a = in.pixelAspectRatio();
    int w = dw.max.x - dw.min.x + 1;
    int h = dw.max.y - dw.min.y + 1;

    Array2D <Rgba> pixels (h, w);
    in.setFrameBuffer (ComputeBasePointer (&pixels[0][0], dw), 1, w);
    in.readPixels (dw.min.y, dw.max.y);

    //
    // Make a preview image
    //

    previewHeight = max (int (h / (w * a) * previewWidth + .5f), 1);
    previewPixels.resizeErase (previewHeight, previewWidth);

    float fx = (previewWidth  > 0)? (float (w - 1) / (previewWidth  - 1)): 1;
    float fy = (previewHeight > 0)? (float (h - 1) / (previewHeight - 1)): 1;
    float m  = Math<float>::pow (2.f, IMATH_NAMESPACE::clamp (exposure + 2.47393f, -20.f, 20.f));

    for (int y = 0; y < previewHeight; ++y)
    {
	for (int x = 0; x < previewWidth; ++x)
	{
	    PreviewRgba &preview = previewPixels[y][x];
	    const Rgba &pixel = pixels[int (y * fy + .5f)][int (x * fx + .5f)];

	    preview.r = gamma (pixel.r, m);
	    preview.g = gamma (pixel.g, m);
	    preview.b = gamma (pixel.b, m);
	    preview.a = int (IMATH_NAMESPACE::clamp (pixel.a * 255.f, 0.f, 255.f) + .5f);
	}
    }
}