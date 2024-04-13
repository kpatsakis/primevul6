AP4_Processor::Process(AP4_ByteStream&   input, 
                       AP4_ByteStream&   output,
                       ProgressListener* listener,
                       AP4_AtomFactory&  atom_factory)
{
    return Process(input, output, NULL, listener, atom_factory);
}