#ifndef COMMAND_LINE_ARGS_INCLUDED
#define COMMAND_LINE_ARGS_INCLUDED

#include <string>

namespace cmd_args
{
        bool get_options(int argc, char* argv[], float* o_x, float* o_y, float* o_z, std::string* o_pathname);
}

#endif // COMMAND_LINE_ARGS_INCLUDED
