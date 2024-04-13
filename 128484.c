PrintPixelFormat(rfbPixelFormat *format)
{
  if (format->bitsPerPixel == 1) {
    rfbClientLog("  Single bit per pixel.\n");
    rfbClientLog(
	    "  %s significant bit in each byte is leftmost on the screen.\n",
	    (format->bigEndian ? "Most" : "Least"));
  } else {
    rfbClientLog("  %d bits per pixel.\n",format->bitsPerPixel);
    if (format->bitsPerPixel != 8) {
      rfbClientLog("  %s significant byte first in each pixel.\n",
	      (format->bigEndian ? "Most" : "Least"));
    }
    if (format->trueColour) {
      rfbClientLog("  TRUE colour: max red %d green %d blue %d"
		   ", shift red %d green %d blue %d\n",
		   format->redMax, format->greenMax, format->blueMax,
		   format->redShift, format->greenShift, format->blueShift);
    } else {
      rfbClientLog("  Colour map (not true colour).\n");
    }
  }
}