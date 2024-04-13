void streamFreeCG(streamCG *cg) {
    raxFreeWithCallback(cg->pel,(void(*)(void*))streamFreeNACK);
    raxFreeWithCallback(cg->consumers,(void(*)(void*))streamFreeConsumer);
    zfree(cg);
}