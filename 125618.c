unsigned long streamLength(const robj *subject) {
    stream *s = subject->ptr;
    return s->length;
}