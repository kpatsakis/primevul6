static s32 find_latest_forward(u32 curFrameHint, u8 *shiftedOrderHints, u8 *usedFrame)
{
	u32 i;
	s32 ref = -1;
	s32 latestOrderHint = 0;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		s32 hint = shiftedOrderHints[i];
		if (!usedFrame[i] && ((u32)hint < curFrameHint) && (ref < 0 || hint >= latestOrderHint)) {
			ref = i;
			latestOrderHint = hint;
		}
	}
	return ref;
}