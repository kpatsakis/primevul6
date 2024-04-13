static s32 av1_get_relative_dist(s32 a, s32 b, AV1State *state)
{
	if (!state->enable_order_hint)
		return 0;
	s32 diff = a - b;
	s32 m = 1 << (state->OrderHintBits - 1);
	diff = (diff & (m - 1)) - (diff & m);
	return diff;
}