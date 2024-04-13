rfbSendFramebufferUpdate(rfbClientPtr cl,
                         sraRegionPtr givenUpdateRegion)
{
    sraRectangleIterator* i=NULL;
    sraRect rect;
    int nUpdateRegionRects;
    rfbFramebufferUpdateMsg *fu = (rfbFramebufferUpdateMsg *)cl->updateBuf;
    sraRegionPtr updateRegion, updateCopyRegion, tmpRegion, cursorRegion;
    sraRect cursorBounds = { 0, 0, 0, 0 };
    int dx, dy;
    rfbBool sendCursorShape = FALSE;
    rfbBool sendCursorPos = FALSE;
    rfbBool cursorIsDrawn = FALSE;

    /*
     * If framebuffer size was changed and the client supports NewFBSize
     * encoding, just send NewFBSize marker and return.
     */

    if (cl->useNewFBSize && cl->newFBSizePending) {
      LOCK(cl->updateMutex);
      cl->newFBSizePending = FALSE;
      UNLOCK(cl->updateMutex);
      cl->rfbFramebufferUpdateMessagesSent++;
      fu->type = rfbFramebufferUpdate;
      fu->nRects = Swap16IfLE(1);
      cl->ublen = sz_rfbFramebufferUpdateMsg;
      if (!rfbSendNewFBSize(cl, cl->screen->width, cl->screen->height)) {
        return FALSE;
      }
      return rfbSendUpdateBuf(cl);
    }

    LOCK(cl->updateMutex);

    /*
     * The modifiedRegion may overlap the destination copyRegion.  We remove
     * any overlapping bits from the copyRegion (since they'd only be
     * overwritten anyway).
     */
    
    sraRgnSubtract(cl->copyRegion,cl->modifiedRegion);

    updateRegion = sraRgnCreateRgn(givenUpdateRegion);
    sraRgnOr(updateRegion,cl->copyRegion);

    /*
     * If the client doesn't support cursor updates, we want to draw
     * the cursor locally, send the update and then undraw it.
     * We only want to do this if the cursor has moved/changed or
     * if it is contained in updateRegion
     *
     * We also want to send an update for the region where the cursor
     * was last drawn, again only if the cursor has moved/changed.
     */

    LOCK(cl->screen->cursorMutex);

    cursorRegion = NULL;
    if (cl->enableCursorUpdates) {
	sendCursorShape = cl->cursorWasChanged;
	sendCursorPos   = cl->cursorWasMoved;
    } else {
	sraRegionPtr lastDrawnCursorRegion;

	if (rfbGetCursorBounds(cl->screen, &cursorBounds)) {
	    cursorRegion = sraRgnCreateRect(cursorBounds.x1, cursorBounds.y1,
					    cursorBounds.x2, cursorBounds.y2);
	}
	
	lastDrawnCursorRegion = sraRgnCreateRect(cl->lastDrawnCursorBounds.x1,
						 cl->lastDrawnCursorBounds.y1,
						 cl->lastDrawnCursorBounds.x2,
						 cl->lastDrawnCursorBounds.y2);

	if (cursorRegion) {
	    sraRgnOr(cursorRegion, lastDrawnCursorRegion);
	    sraRgnDestroy(lastDrawnCursorRegion);
	} else {
	    cursorRegion = lastDrawnCursorRegion;
	}

	if (cl->cursorWasChanged || cl->cursorWasMoved)
	    sraRgnOr(updateRegion, cursorRegion);
    }

    /*
     * The client is interested in the region requestedRegion.  The region
     * which should be updated now is the intersection of requestedRegion
     * and the union of modifiedRegion and copyRegion.  If it's empty then
     * no update is needed.
     */

    if (!sraRgnAnd(updateRegion, cl->requestedRegion) &&
	!sendCursorShape && !sendCursorPos) {
	sraRgnDestroy(updateRegion);
	if (cursorRegion)
	    sraRgnDestroy(cursorRegion);
	UNLOCK(cl->updateMutex);
	UNLOCK(cl->cursorMutex);
	return TRUE;
    }

    /*
     * Put up the cursor if any part of it is in updateRegion.
     */

    if (cursorRegion) {
	tmpRegion = sraRgnCreateRgn(cursorRegion);

	if (sraRgnAnd(tmpRegion, updateRegion)) {
	    rfbDrawCursor(cl->screen, &cursorBounds);
	    cursorIsDrawn = TRUE;

	    cl->cursorWasMoved = FALSE;
	    cl->cursorWasChanged = FALSE;
	    cl->lastDrawnCursorBounds = cursorBounds;

	    sraRgnOr(cl->modifiedRegion, cursorRegion);
	    
	    sraRgnOr(updateRegion, cursorRegion);
	    sraRgnAnd(updateRegion, cl->requestedRegion);
	}

	sraRgnDestroy(tmpRegion);
	sraRgnDestroy(cursorRegion);
    }

    /*
     * We assume that the client doesn't have any pixel data outside the
     * requestedRegion.  In other words, both the source and destination of a
     * copy must lie within requestedRegion.  So the region we can send as a
     * copy is the intersection of the copyRegion with both the requestedRegion
     * and the requestedRegion translated by the amount of the copy.  We set
     * updateCopyRegion to this.
     */

    updateCopyRegion = sraRgnCreateRgn(cl->copyRegion);
    sraRgnAnd(updateCopyRegion,cl->requestedRegion);
    tmpRegion = sraRgnCreateRgn(cl->requestedRegion);
    sraRgnOffset(tmpRegion,cl->copyDX,cl->copyDY);
    sraRgnAnd(updateCopyRegion,tmpRegion);
    sraRgnDestroy(tmpRegion);
    dx = cl->copyDX;
    dy = cl->copyDY;

    /*
     * Next we remove updateCopyRegion from updateRegion so that updateRegion
     * is the part of this update which is sent as ordinary pixel data (i.e not
     * a copy).
     */

    sraRgnSubtract(updateRegion,updateCopyRegion);

    /*
     * Finally we leave modifiedRegion to be the remainder (if any) of parts of
     * the screen which are modified but outside the requestedRegion.  We also
     * empty both the requestedRegion and the copyRegion - note that we never
     * carry over a copyRegion for a future update.
     */

     sraRgnOr(cl->modifiedRegion,cl->copyRegion);
     sraRgnSubtract(cl->modifiedRegion,updateRegion);
     sraRgnSubtract(cl->modifiedRegion,updateCopyRegion);

     sraRgnMakeEmpty(cl->requestedRegion);
     sraRgnMakeEmpty(cl->copyRegion);
     cl->copyDX = 0;
     cl->copyDY = 0;
   
     UNLOCK(cl->updateMutex);
   
   /*
     * Now send the update.
     */

    cl->rfbFramebufferUpdateMessagesSent++;

    if (cl->preferredEncoding == rfbEncodingCoRRE) {
        nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
            nUpdateRegionRects += (((w-1) / cl->correMaxWidth + 1)
                                     * ((h-1) / cl->correMaxHeight + 1));
        }
	sraRgnReleaseIterator(i);
#ifdef VINO_HAVE_ZLIB
    } else if (cl->preferredEncoding == rfbEncodingZlib) {
	nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
	    nUpdateRegionRects += (((h-1) / (ZLIB_MAX_SIZE( w ) / w)) + 1);
	}
#ifdef VINO_HAVE_JPEG
    } else if (cl->preferredEncoding == rfbEncodingTight) {
	nUpdateRegionRects = 0;

        for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
            int x = rect.x1;
            int y = rect.y1;
            int w = rect.x2 - x;
            int h = rect.y2 - y;
	    int n = rfbNumCodedRectsTight(cl, x, y, w, h);
	    if (n == 0) {
		nUpdateRegionRects = 0xFFFF;
		break;
	    }
	    nUpdateRegionRects += n;
	}
	sraRgnReleaseIterator(i);
#endif
#endif
    } else {
        nUpdateRegionRects = sraRgnCountRects(updateRegion);
    }

    fu->type = rfbFramebufferUpdate;
    if (nUpdateRegionRects != 0xFFFF) {
	if(cl->screen->maxRectsPerUpdate>0
	   /* CoRRE splits the screen into smaller squares */
	   && cl->preferredEncoding != rfbEncodingCoRRE
#ifdef VINO_HAVE_ZLIB
	   /* Zlib encoding splits rectangles up into smaller chunks */
	   && cl->preferredEncoding != rfbEncodingZlib
#ifdef VINO_HAVE_JPEG
	   /* Tight encoding counts the rectangles differently */
	   && cl->preferredEncoding != rfbEncodingTight
#endif
#endif /* VINO_HAVE_ZLIB */
	   && nUpdateRegionRects>cl->screen->maxRectsPerUpdate) {
	    sraRegion* newUpdateRegion = sraRgnBBox(updateRegion);
	    sraRgnDestroy(updateRegion);
	    updateRegion = newUpdateRegion;
	    nUpdateRegionRects = sraRgnCountRects(updateRegion);
	}
	fu->nRects = Swap16IfLE((uint16_t)(sraRgnCountRects(updateCopyRegion) +
					   nUpdateRegionRects +
					   !!sendCursorShape + !!sendCursorPos));
    } else {
	fu->nRects = 0xFFFF;
    }
    cl->ublen = sz_rfbFramebufferUpdateMsg;

    UNLOCK(cl->cursorMutex);

   if (sendCursorShape) {
	cl->cursorWasChanged = FALSE;
	if (!rfbSendCursorShape(cl))
	    goto tx_error;
    }
   
   if (sendCursorPos) {
	cl->cursorWasMoved = FALSE;
	if (!rfbSendCursorPos(cl))
	    goto tx_error;
    }
   
    if (!sraRgnEmpty(updateCopyRegion)) {
	if (!rfbSendCopyRegion(cl,updateCopyRegion,dx,dy))
	    goto tx_error;
    }

    for(i = sraRgnGetIterator(updateRegion); sraRgnIteratorNext(i,&rect);){
        int x = rect.x1;
        int y = rect.y1;
        int w = rect.x2 - x;
        int h = rect.y2 - y;

        cl->rfbRawBytesEquivalent += (sz_rfbFramebufferUpdateRectHeader
                                      + w * (cl->format.bitsPerPixel / 8) * h);

        switch (cl->preferredEncoding) {
        case rfbEncodingRaw:
            if (!rfbSendRectEncodingRaw(cl, x, y, w, h))
		goto tx_error;
            break;
        case rfbEncodingRRE:
            if (!rfbSendRectEncodingRRE(cl, x, y, w, h))
		goto tx_error;
            break;
        case rfbEncodingCoRRE:
            if (!rfbSendRectEncodingCoRRE(cl, x, y, w, h))
		goto tx_error;
            break;
        case rfbEncodingHextile:
            if (!rfbSendRectEncodingHextile(cl, x, y, w, h))
		goto tx_error;
            break;
#ifdef VINO_HAVE_ZLIB
	case rfbEncodingZlib:
	    if (!rfbSendRectEncodingZlib(cl, x, y, w, h))
		goto tx_error;
	    break;
#ifdef VINO_HAVE_JPEG
	case rfbEncodingTight:
	    if (!rfbSendRectEncodingTight(cl, x, y, w, h))
		goto tx_error;
	    break;
#endif
#endif
#ifdef VINO_HAVE_ZLIB
       case rfbEncodingZRLE:
           if (!rfbSendRectEncodingZRLE(cl, x, y, w, h))
	       goto tx_error;
           break;
#endif
        }
    }

    if (cursorIsDrawn)
	rfbUndrawCursor(cl->screen, &cursorBounds);

    if ( nUpdateRegionRects == 0xFFFF &&
	 !rfbSendLastRectMarker(cl) )
	goto tx_error;

    if (!rfbSendUpdateBuf(cl))
	goto tx_error;

    sraRgnReleaseIterator(i);
    sraRgnDestroy(updateCopyRegion);
    sraRgnDestroy(updateRegion);
    
    return TRUE;

 tx_error:
    if (cursorIsDrawn)
	rfbUndrawCursor(cl->screen, &cursorBounds);
    if (i)
	sraRgnReleaseIterator(i);

    sraRgnDestroy(updateCopyRegion);
    sraRgnDestroy(updateRegion);

    return FALSE;
}