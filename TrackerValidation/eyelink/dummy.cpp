// Dummy methods for the eyelink system. These will be used if the
// eyelink_core64 lib is not present to allow the system to compile. If these
// dummy methods are used, connecting to the eyelink will not work.

#include "core_expt.h"
#include "eyelink.h"

#include <stdexcept>
#include <iostream>

namespace
{
    void eyelinkInstructions()
    {
        std::cerr << "ERROR: Eyelink libraries have not been loaded. To use the "
                  << "Eyelink tracker, you need to add \"eyelink_core64.lib\" "
                  << "to the \"eyelink\" folder. This file can be downloaded "
                  << "from https://www.sr-support.com/thread-13.html after "
                  << "creating an account on their system. To get the file, "
                  << "download the \"Eyelink Developers Kit\" and run the "
                  << "installer. Libraries will be installed to your hard "
                  << "drive, possibly at C:\\Program Files (x86)\\SR Research\\EyeLink\\libs\\x64. "
                  << "Copy the file from that directory into the \"eyelink\" "
                  << "directory in this project, clean all and remake all."
                  << std::endl
                  << "Note: this eye tracker is currently only supported under Windows."
                  << std::endl;
    }
}

void ELCALLTYPE close_eyelink_system(void)
{
    ::eyelinkInstructions();
}

INT16 ELCALLTYPE eyecmd_printf(const char *, ...)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE eyelink_newest_sample(void FARTYPE *)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE eyelink_position_prescaler(void)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE eyelink_wait_for_block_start(UINT32, INT16, INT16)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE open_eyelink_connection(INT16)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE set_eyelink_address(char *addr)
{
    ::eyelinkInstructions();
    return -1;
}

INT16 ELCALLTYPE start_recording(INT16, INT16, INT16, INT16)
{
    ::eyelinkInstructions();
    return -1;
}

void ELCALLTYPE stop_recording(void)
{
    ::eyelinkInstructions();
}
