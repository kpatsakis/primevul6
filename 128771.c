get_stream(struct snd_line6_pcm *line6pcm, int direction)
{
	return (direction == SNDRV_PCM_STREAM_PLAYBACK) ?
		&line6pcm->out : &line6pcm->in;
}