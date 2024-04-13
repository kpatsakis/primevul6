robj *createObjectFromStreamID(streamID *id) {
    return createObject(OBJ_STRING, createStreamIDString(id));
}