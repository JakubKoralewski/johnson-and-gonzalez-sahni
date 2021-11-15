// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include "imp1.c"

void print_help_menu() {
	printf(
		"\nThis is the help menu.\n"
		"Implementation 1 program by Jakub Koralewski.\n"
		"Example usages:\n"
		"\t- \"imp1.exe erd ./input.txt\"\n"
		"\t\t deadlines and weights are completely ignored for ERD\n"
		"\t- \".\\imp1.exe EDD .\\input.txt\"\n"
		"\t\t weights are completely ignored for EDD\n"
		"\t- \"imp1.exe spt input.txt\"\n"
		"\t\t deadlines and weights are completely ignored for SPT\n"
		"\t- \"imp1.exe WsPT input.txt\"\n"
		"\t\t deadlines are completely ignored for WSPT\n\n"
		"Where the contents of the input file are a subset of the CSV format defined as follows:\n"
		"\tThe first line is the header line. The allowed values are: 'p','r','d','w' which respectively correspond to:\n"
		"\t\t- processing time\n"
		"\t\t- ready time\n"
		"\t\t- deadline /due-date\n"
		"\t\t- weight\n"
		"\tOn each line put the same number of values separated by commas. Each column corresponding to the defined header.\n"
		"\tOnly non-negative integers are allowed (inc. weight). With the following exceptions:\n"
		"\t\tGive \"inf\" in place of value to signify infinity. (Actual value implementation defined)\n"
		"\tJobs are numbered in the output based on the line on which they were defined in the file.\n"
		"\tExamples:"
		"\n"
		"\t\tp,r,d\n"
		"\t\t1,2,inf\n"
		"\t\t2,3,inf\n"
		"\t\t1,4,inf\n"
		"\n"
		"\t\tp,r,w\n"
		"\t\t1,2,2\n"
		"\t\t2,3,3\n"
		"\t\t1,4,1\n"
	);
}

int main(int argc, char** argv) {
	int rv = 0;
	Input input = {.buffer = NULL};
	Schedule schedule = {.schedule = NULL};
	if(argc >= 2) {
		if(strcasecmp(argv[1], "help") == 0) {
			print_help_menu();
			return 0;
		}
	}
	if(argc != 3) {
		fprintf(stderr, "Invalid number of arguments. (%d) should be 2 (mode, path)", argc-1);
		rv = 123;
		goto suggest_help;
	}
	char* path = argv[2];
	if (!path) {
		fprintf(stderr, "No path given.");
		goto suggest_help;
	}
	if ((rv = read_input_from_file(path, &input))) {
		fprintf(stderr, "Couldn't read file %s\n", path);
		goto cleanup;
	}
	char* mode = argv[1];
	switch((rv = imp1(&schedule, &input, mode))) {
		case 0:
			// Success
			schedule_print(&schedule);
			goto cleanup;
		case ERROR_WITH_HELP:
			// Error that can be fixed by user learning about how to use this program
			goto suggest_help;
		default:
			// Any other error
			goto cleanup;
	}
suggest_help:
	printf("\nType \"help\" to open the help menu!\n");
	// Also cleans up, no return

cleanup:
	if(input.buffer != NULL) {
		 free(input.buffer);
		 input.buffer = NULL;
	}
	// Null check required cause goto cleanup also before schedule malloc, relies on zero initialization w/ {0}
	if(schedule.schedule != NULL) {
		free(schedule.schedule);
	}
	return rv;

}
