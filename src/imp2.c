// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include "schedule.c"

int nonincreasing_processing_time(Job* a, Job* b) {
	return a->prdw[P] < b->prdw[P];
}

MACHINE first_available_machine(const MACHINE num_machines, const TIME* completion_time_per_machine) {
	TIME min_completion_time = TIME_MAX;
	MACHINE index_of_machine_with_min_c = 0;
	for(MACHINE j = 1; j <= num_machines; j++) {
		if(completion_time_per_machine[j] < min_completion_time) {
			min_completion_time = completion_time_per_machine[j];
			index_of_machine_with_min_c = j;
		}
	}

	return index_of_machine_with_min_c;
}

int list_scheduling(const MACHINE num_machines, Input* inout_input, Schedule* inout_schedule) {
	inout_schedule->input = inout_input;
	inout_schedule->schedule = malloc(inout_input->length * sizeof(Job));

	// Add one to be able to operate in 1..=num_machines range and not worry about error-prone -1 conversions
	TIME* completion_time_per_machine = calloc((num_machines + 1), sizeof(TIME));
	for(size_t i = 0; i < inout_input->length; i++) {
		Job* job = &inout_input->buffer[i];
		MACHINE machine_id = first_available_machine(num_machines, completion_time_per_machine);

		// Create job
		TIME starting_time = completion_time_per_machine[machine_id];
		starting_time = job->prdw[R] > starting_time ? job->prdw[R] : starting_time;
		ScheduledJob schedule_data = {
			.start = starting_time,
			.end = starting_time + job->prdw[P],
		};
		starting_time = schedule_data.end;
		inout_schedule->schedule[i] = schedule_data;

		job->m_id = machine_id;
		completion_time_per_machine[machine_id] += job->prdw[P];
	}
	free(completion_time_per_machine);
}

int imp2(MACHINE num_machines, Schedule* out_schedule, Input* in_input, const char* mode) {
	int rv = 0;
	if (strcasecmp(mode, "ls") == 0) {
		if(require_set(in_input, 1 << P)) {
			fprintf(stderr, "LS requires processing times!");
			exit(1);
		};
		list_scheduling(num_machines, in_input, out_schedule);
	} else if (strcasecmp(mode, "lpt") == 0) {
		if(require_set(in_input, 1 << P)) {
			fprintf(stderr, "LPT requires processing times!");
			exit(1);
		};
		qsort(in_input->buffer, in_input->length, sizeof(Job), (COMPARE_FUNC) nonincreasing_processing_time);
		list_scheduling(num_machines, in_input, out_schedule);
	} else {
		fprintf(stderr, "Invalid mode %s\n", mode);
		return ERROR_WITH_HELP;
	}
//	schedule_new(out_schedule, in_input);
	return rv;
}

