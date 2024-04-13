LineBufferTask::~LineBufferTask ()
{
    //
    // Signal that the line buffer is now free
    //

    _lineBuffer->post ();
}