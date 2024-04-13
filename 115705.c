AP4_Processor::Process(AP4_ByteStream&   fragments, 
                       AP4_ByteStream&   output,
                       AP4_ByteStream&   init,
                       ProgressListener* listener,
                       AP4_AtomFactory&  atom_factory)
{
    return Process(init, output, &fragments, listener, atom_factory);
}