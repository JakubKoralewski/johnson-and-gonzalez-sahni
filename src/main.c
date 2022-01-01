// Jakub Koralewski 452490
// McNaughton and Modified SPT
#include "imp4.c"

void print_help_menu() {
	printf(
		"\nThis is the help menu.\n"
		"Implementation 4 program by Jakub Koralewski.\n"
		"Example usages:\n"
		"\t- \"imp4.exe john 3 ./test/inputs/johnsons_all_in_1st_part.txt\"\n"
		"\t- \"imp4.exe spt 2 input.txt\"\n"
		"\t\t only processing times are used for McNaughton's and modified spt \n"
		"Where the contents of the input file are a subset of the CSV format defined as follows:\n"
		"\tThe first line is the header line. The allowed comma separated values are: 'p','r','d','w' which respectively correspond to:\n"
		"\t\t- processing time\n"
		"\t\t- ready time\n"
		"\t\t- deadline /due-date\n"
		"\t\t- weight\n"
		"\tThe values in the header can be put in any order, but remember to order the values below in same order.\n"
		"\tOn each line put the same number of values as in the header. separated by commas. Each column corresponding to the defined header.\n"
		"\tOnly non-negative integers are allowed (inc. weight). With the following exceptions:\n"
		"\t\tGive \"inf\" in place of value to signify infinity. (Actual value implementation defined)\n"
		"\tJobs are numbered in the output based on the line on which they were defined in the file.\n"
		"\tRunning a scheduling rule on a file without the data which is used to sort results in "
		"undefined behavior, as the sorting algorithm used may be unstable when sorting equal values.\n"
		"\tFiles not containing the required values for the given rules will error.\n"
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
	Input input = {.operations = NULL};
	Schedule schedule = {.schedule = NULL};
	if(argc > 4) {
		if(strcasecmp(argv[1], "help") == 0) {
			print_help_menu();
			return 0;
		}
	}
	if(argc != 4) {
		fprintf(stderr, "Invalid number of arguments (%d) should be 3 (number_of_machines, mode, path)", argc-1);
		rv = 123;
		goto suggest_help;
	}
	char* path = argv[3];
	if (!path) {
		fprintf(stderr, "No path given.");
		goto suggest_help;
	}
	if ((rv = read_input_from_file(path, &input))) {
		fprintf(stderr, "Couldn't read file %s\n", path);
		goto cleanup;
	}
	char* mode = argv[1];

	MACHINE number_of_machines = strtoul(argv[2], NULL, 10);
	if(number_of_machines == 0) {
		fprintf(stderr, "Number of machines must not be 0\n");
		goto cleanup;
	}
	switch((rv = imp4(number_of_machines, &schedule, &input, mode))) {
		case 0:
			// Success
			schedule_print(&schedule);
			TIME c_max = 0;
			if(min_max_time(
				schedule.schedule, sizeof(ScheduledJob), schedule.length,
				(GET_INT) get_end, (MIN_MAX_CMP_FUNC) max_time, &c_max)
			){
				fprintf(stderr, "Invalid data");
				rv = 1;
				goto cleanup;
			}
			printf("\nc_max = %f", c_max);

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
	if(input.operations != NULL) {
		 free(input.operations);
		 input.operations = NULL;
	}
	// Null check required cause goto cleanup also before schedule malloc, relies on zero initialization w/ {0}
	if(schedule.schedule != NULL) {
		free(schedule.schedule);
	}
	return rv;

}
