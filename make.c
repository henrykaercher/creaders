 //This nob.h has some changes that will differ from the original repository
//Changes: 'nob_cancer_log' has been removed

#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char *argv[]){
	NOB_GO_REBUILD_URSELF(argc, argv);

	if(!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
	
	Nob_Cmd cmd = {0};

#if !defined(_MSC_VER)
	// On POSIX
	nob_cmd_append(&cmd, "cc", "-Wall", "-std=c11", "-Wextra", "-o", BUILD_FOLDER"build", SRC_FOLDER"main.c");
#else
	// On MSVC
	// this line its just hanging here, I dont really know MSCV, just keeping for the intention of an alternative
	nob_cmd_append(&cmd, "cl", "/I.", "/std:c11", "/Fe:"BUILD_FOLDER"buid", SRC_FOLDER"main.c");
#endif // _MSC_VER
	
	if(!nob_cmd_run(&cmd)) return 1;

	nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_output(&cmd, BUILD_FOLDER "mk_reader");
    nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
	if (!nob_cmd_run(&cmd)) return 1;

    return 0;
}
