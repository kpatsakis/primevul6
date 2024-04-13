static void av1_set_frame_refs(AV1State *state, u8 last_frame_idx, u8 gold_frame_idx, s8 *ref_frame_idx)
{
	u32 i;
	u8 usedFrame[AV1_NUM_REF_FRAMES];
	u8 shiftedOrderHints[AV1_NUM_REF_FRAMES];

	for (i = 0; i < AV1_REFS_PER_FRAME; i++)
		ref_frame_idx[i] = -1;

	ref_frame_idx[AV1_LAST_FRAME - AV1_LAST_FRAME] = last_frame_idx;
	ref_frame_idx[AV1_GOLDEN_FRAME - AV1_LAST_FRAME] = gold_frame_idx;

	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		usedFrame[i] = 0;
	}

	usedFrame[last_frame_idx] = 1;
	usedFrame[gold_frame_idx] = 1;
	u32 curFrameHint = 1 << (state->OrderHintBits - 1);

	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		shiftedOrderHints[i] = curFrameHint + av1_get_relative_dist(state->RefOrderHint[i], state->frame_state.order_hint, state);
	}

	u8 lastOrderHint = shiftedOrderHints[last_frame_idx];
	u8 goldOrderHint = shiftedOrderHints[gold_frame_idx];

	//It is a requirement of bitstream conformance that lastOrderHint is strictly less than curFrameHint.
	if (lastOrderHint >= curFrameHint) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] non conformant bitstream detected while setting up frame refs: lastOrderHint(%d) shall be stricly less than curFrameHint(%d)\n", lastOrderHint, curFrameHint));
	}
	//It is a requirement of bitstream conformance that goldOrderHint is strictly less than curFrameHint.
	if (goldOrderHint >= curFrameHint) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] non conformant bitstream detected while setting up frame refs: goldOrderHint(%d) shall be stricly less than curFrameHint(%d)\n", lastOrderHint, curFrameHint));
	}

	//find_latest_backward() {
	s32 ref = -1;
	s32 latestOrderHint = 0;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		s32 hint = shiftedOrderHints[i];
		if (!usedFrame[i] && ((u32)hint >= curFrameHint) && (ref < 0 || hint >= latestOrderHint)) {
			ref = i;
			latestOrderHint = hint;
		}
	}
	if (ref >= 0) {
		ref_frame_idx[AV1_ALTREF_FRAME - AV1_LAST_FRAME] = ref;
		usedFrame[ref] = 1;
	}
	//find_earliest_backward() for BWDREF_FRAME
	ref = -1;
	s32 earliestOrderHint = 0;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		s32 hint = shiftedOrderHints[i];
		if (!usedFrame[i] && ((u32)hint >= curFrameHint) && (ref < 0 || hint < earliestOrderHint)) {
			ref = i;
			earliestOrderHint = hint;
		}
	}
	if (ref >= 0) {
		ref_frame_idx[AV1_BWDREF_FRAME - AV1_LAST_FRAME] = ref;
		usedFrame[ref] = 1;
	}

	//find_earliest_backward() for ALTREF2_FRAME
	ref = -1;
	earliestOrderHint = 0;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		s32 hint = shiftedOrderHints[i];
		if (!usedFrame[i] && ((u32)hint >= curFrameHint) && (ref < 0 || hint < earliestOrderHint)) {
			ref = i;
			earliestOrderHint = hint;
		}
	}
	if (ref >= 0) {
		ref_frame_idx[AV1_ALTREF2_FRAME - AV1_LAST_FRAME] = ref;
		usedFrame[ref] = 1;
	}

	//The remaining references are set to be forward references in anti-chronological order as follows:

	const u8 Ref_Frame_List[AV1_REFS_PER_FRAME - 2] = {
		AV1_LAST2_FRAME, AV1_LAST3_FRAME, AV1_BWDREF_FRAME, AV1_ALTREF2_FRAME, AV1_ALTREF_FRAME
	};

	for (i = 0; i < AV1_REFS_PER_FRAME - 2; i++) {
		u8 refFrame = Ref_Frame_List[i];
		if (ref_frame_idx[refFrame - AV1_LAST_FRAME] < 0) {
			s32 last_ref = find_latest_forward(curFrameHint, shiftedOrderHints, usedFrame);
			if (last_ref >= 0) {
				ref_frame_idx[refFrame - AV1_LAST_FRAME] = last_ref;
				usedFrame[last_ref] = 1;
			}
		}
	}
	//Finally, any remaining references are set to the reference frame with smallest output order as follows:
	ref = -1;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		s32 hint = shiftedOrderHints[i];
		if (ref < 0 || hint < earliestOrderHint) {
			ref = i;
			earliestOrderHint = hint;
		}
	}
	for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
		if (ref_frame_idx[i] < 0) {
			ref_frame_idx[i] = ref;
		}
	}
}