// Software version
// Written by Tim Murphy <tim@murphy.org> 2021

#ifndef VERSION_H
#define VERSION_H

// macro magic to stringify the hash
// need two levels of macros or the substitution doesn't work
#define _VER_STR(X) _VER_STR2(X)
#define _VER_STR2(X) #X

#ifndef LATEST_GIT_HASH
#define LATEST_GIT_HASH "<unknown>"
#endif

const char * const softwareVersion(void)
{
    static const char * const v = "git hash " _VER_STR(LATEST_GIT_HASH);
    return v;
}

#endif // not defined VERSION_H