// Jakub Koralewski 452490
// Johnson, Gonzales-Sahni
#include "schedule.c"

// Johnson's algorithm

typedef struct {
	Operation operation;
	// discards both operations, since an index applies to two operations
	int* map_index_to_whether_inserted;
} MinOp;

void min_op(MinOp* prev, const Operation* cur) {
	if(prev->map_index_to_whether_inserted[(INDEX)cur->prdwjm[J] - 1] != 0) {
		// if already added abort
		return;
	}
	if(prev->operation.prdwjm[P] < cur-> prdwjm[P]) {
		// if prev is smaller abort
		return;
	}
	if(prev->operation.prdwjm[P] == cur->prdwjm[P] && cur->prdwjm[J] > prev->operation.prdwjm[J]) {
		// in case of ties we order the item with the smallest index first,
		// therefore if P equal & prev index larger -> abort
		return;
	}
	prev->operation = *cur;
}

/// Num machines is not required as johnsons only works for 2 machines
int johnsons(Input* inout_input, Schedule* inout_schedule, INDEX* inout_final_sequence) {
	// List operation processing times p1j and p2j in two columns
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	size_t num_jobs = inout_input->length / 2;
	// + 1, in case of odd number of jobs one of the indices in the middle will be 0
	size_t final_sequence_len = num_jobs + 1;
	size_t cursor_end_of_1st_part = 0;
	size_t cursor_end_of_2nd_part = final_sequence_len / 2;

	MinOp min = {
		.operation = {
			.prdwjm = {TIME_MAX, 0, 0, 0, 0, 0}
		},
		.map_index_to_whether_inserted = calloc(num_jobs, sizeof(int)),
	};

	size_t num_jobs_left_to_check = num_jobs;

	// 5. Repeat steps 2-4 for remaining entries, working from both ends to the middle of the final sequence.
	while (num_jobs_left_to_check-- != 0) {
		// 2. Scan all the entries and find the smallest value
		if(min_max_time(
			inout_input->operations, sizeof(Operation), inout_input->length,
			&min, (MIN_MAX_CMP_FUNC) min_op
		)) {
			fprintf(stderr, "invalid data");
			exit(1);
		}
		INDEX index_to_put_index_in;
		// 3. If the value is in the first column,
		if(min.operation.prdwjm[M] == 1) {
			// put index of that job at the begin of the final sequence else
			index_to_put_index_in = cursor_end_of_1st_part;
			cursor_end_of_1st_part++;

		} else if (min.operation.prdwjm[M] == 2) {
			// put it in the end of that sequence
			index_to_put_index_in = cursor_end_of_2nd_part;
			cursor_end_of_2nd_part++;
		} else {
			fprintf(stderr, "Corrupted state");
			exit(1);
		}
		inout_final_sequence[index_to_put_index_in] = (INDEX) min.operation.prdwjm[J];

		// 4. Delete both operation processing times from the columns
		min.map_index_to_whether_inserted[(INDEX)min.operation.prdwjm[J] - 1] = 1;
		min.operation.prdwjm[P] = TIME_MAX;
	}
	free(min.map_index_to_whether_inserted);

	// We get rid of the possible 0 in the middle in case of odd number of jobs
	memmove(
		inout_final_sequence + cursor_end_of_1st_part,
		inout_final_sequence + (final_sequence_len / 2),
		cursor_end_of_2nd_part - (final_sequence_len / 2)
	);
	final_sequence_len--;
	for(int i = 0; i < final_sequence_len; i++) {
		// we duplicate the same for the other machine
		inout_final_sequence[i + final_sequence_len] = inout_final_sequence[i];
	}
	printf("\n");

	return 0;
}

// Gonzalez-Sahni (Brucker) algorithm

int nondecreasing_index_then_nondecreasing_machine(const Operation* a, const Operation* b) {
	if(a->prdwjm[J] > b->prdwjm[J]) return 1;
	if(a->prdwjm[J] == b->prdwjm[J]) {
		return a->prdwjm[M] > b->prdwjm[M];
	}
	return 0;
}

/// Also works for 2 machines only
int gonzalez_sahni(Input* inout_input, Schedule* inout_schedule, INDEX* inout_final_sequence) {
	qsort(
		inout_input->operations, inout_input->length, sizeof(Operation),
		(COMPARE_FUNC) nondecreasing_index_then_nondecreasing_machine
	);

	// List operation processing times p1j and p2j in two columns
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	size_t num_jobs = inout_input->length / 2;

	int* map_index_to_whether_job_belongs_to_first_set = calloc(num_jobs, sizeof(int));
	size_t num_in_1st_set = 0;
	TIME k = 0;
	MACHINE machine_with_k = 0;
	INDEX r = -1;

	for(int i =0; i<num_jobs; i += 2) {
		Operation* op_1st_machine = &inout_input->operations[i];
		Operation* op_2nd_machine = &inout_input->operations[i + 1];
		if(op_1st_machine->prdwjm[M] != 1 || op_2nd_machine->prdwjm[M] != 2) {
			fprintf(stderr, "Corrupted state: sorting didn't work?");
			exit(1);
		}
		if(op_1st_machine->prdwjm[P] > k) {
			k = op_1st_machine->prdwjm[P];
			machine_with_k = 1;
			r = (INDEX)op_1st_machine->prdwjm[J];
		}
		if(op_2nd_machine->prdwjm[P] > k) {
			k = op_2nd_machine->prdwjm[P];
			machine_with_k = 2;
			r = (INDEX)op_2nd_machine->prdwjm[J];
		}
		if(op_1st_machine->prdwjm[P] >= op_2nd_machine->prdwjm[P]) {
			map_index_to_whether_job_belongs_to_first_set[i] = 1;
			num_in_1st_set++;
		}
	}
	size_t num_in_2nd_set = num_jobs - num_in_1st_set;

	// + 1, in case of odd number of jobs one of the indices in the middle will be 0
	size_t num_operations = inout_input->length;
	INDEX* final_sequence_of_1st_m_1st_part = inout_final_sequence;
	INDEX* cursor_1st_m_1st_part = final_sequence_of_1st_m_1st_part;
	INDEX* cursor_1st_m_2nd_part;

	INDEX* final_sequence_of_2nd_m_1st_part = final_sequence_of_1st_m_1st_part + num_operations / 2;
	INDEX* cursor_2nd_m_1st_part = final_sequence_of_2nd_m_1st_part;
	INDEX* cursor_2nd_m_2nd_part;

	if(machine_with_k == 1) {
		size_t num_in_1st_set_without_r = num_in_1st_set == 0 ? 0 : num_in_1st_set - 1;
		// m_1=(...,J^2,...)
		cursor_1st_m_2nd_part = cursor_1st_m_1st_part + num_in_1st_set_without_r;
		// m_2=(r,...
		*(cursor_2nd_m_1st_part++) = r;
		// m_2=(...,J^2)
		cursor_2nd_m_2nd_part = cursor_2nd_m_1st_part + num_in_1st_set_without_r;
	} else if (machine_with_k == 2) {
		size_t num_in_2nd_set_without_r = num_in_2nd_set == 0 ? 0 : num_in_2nd_set - 1;
		// m_1=(r,...
		*(cursor_1st_m_1st_part++) = r;
		// m_1=(...,J^1)
		cursor_1st_m_2nd_part = cursor_1st_m_1st_part + num_in_2nd_set_without_r;
		// m_2=(...,J^1,...)
		cursor_2nd_m_2nd_part = cursor_2nd_m_1st_part + num_in_2nd_set_without_r;
	} else {
		fprintf(stderr, "Corrupted state: 2 machines only!");
		exit(1);
	}

	for(int i = 0; i < num_jobs; i++) {
		size_t job_index = i + 1;
		if (job_index == r) {
			continue;
		}
		// can be assumed operations are sorted wrt index first, then machines
		int is_in_first_set = map_index_to_whether_job_belongs_to_first_set[i];
		if (machine_with_k == 1) {
			if (is_in_first_set) {
				*(cursor_1st_m_1st_part++) = job_index; // J^1 - {r}
				*(cursor_2nd_m_1st_part++) = job_index; // J^1 - {r}
			} else {
				*(cursor_1st_m_2nd_part++) = job_index; // J^2
				*(cursor_2nd_m_2nd_part++) = job_index; // J^2
			}
		} else {
			if (is_in_first_set) {
				*(cursor_1st_m_2nd_part++) = job_index; // J^1
				*(cursor_2nd_m_2nd_part++) = job_index; // J^1
			} else {
				*(cursor_1st_m_1st_part++) = job_index; // J^2 - {r}
				*(cursor_2nd_m_1st_part++) = job_index; // J^2 - {r}
			}
		}
	}
	if(machine_with_k == 1) {
		*(cursor_1st_m_2nd_part++) = r; // m_1 = (...,r)
	} else {
		*(cursor_2nd_m_2nd_part++) = r; // m_2 = (...,r)
	}

	free(map_index_to_whether_job_belongs_to_first_set);
	return 0;
}

// Jackson's

int jacksons(Input* inout_input, Schedule* inout_schedule, INDEX* inout_final_sequence) {
	johnsons(inout_input, inout_schedule, inout_final_sequence);
	fprintf(stderr, "I don't understand how a route fits into this");

	exit(1);
}

int begins_with(const char* to_check, const char* should_be) {
	return strncmp(to_check, should_be, strlen(should_be)) == 0;
}

int imp4(MACHINE num_machines, Schedule* out_schedule, Input* in_input, const char* mode) {
	int rv = 0;
	if (require_set(in_input, 1 << P)) {
		fprintf(stderr, "All algorithms require processing times!");
		exit(1);
	}
	size_t num_operations = in_input->length;
	// first half is for first machine, second half for 2nd machine
	INDEX* final_sequence = malloc(num_operations * sizeof(INDEX));
	if (begins_with(mode, "john")) {
		if(num_machines == 3) {
			fprintf(stderr, "TODO: slide 14 lecture 10, special case for F3");
			exit(1);
		} else if(num_machines != 2) {
			fprintf(stderr, "Johnson's algorithm only works for 2 machines!");
			exit(1);
		}

		johnsons(in_input, out_schedule, final_sequence);
	} else if (begins_with(mode, "gon") || begins_with(mode, "br")){
		gonzalez_sahni(in_input, out_schedule, final_sequence);
	} else if (begins_with(mode, "jack")) {
		jacksons(in_input, out_schedule, final_sequence);
	} else {
		fprintf(stderr, "Invalid mode %s\n", mode);
		rv = ERROR_WITH_HELP;
		goto cleanup;
	}

	INDEX* cursor = final_sequence;
	size_t num_jobs = num_operations / 2;
	for(int m = 0; m < 2; m++) {
		printf("Sequence of jobs on machine %d\n", m + 1);
		for(int i = 0; i < num_jobs; i++) {
			INDEX j = *(cursor++);
			printf("%d, ", j);
		}
		printf("\n");
	}

cleanup:
	free(final_sequence);
	return rv;
}

