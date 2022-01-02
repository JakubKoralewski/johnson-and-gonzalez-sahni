// Jakub Koralewski 452490
// McNaughton and modified SPT
#include "schedule.c"

int nondecreasing_processing_time(Operation* a, Operation* b) {
	return a->prdwjm[P] > b->prdwjm[P];
}

int first_available_machine(const MACHINE num_machines, const TIME* completion_time_per_machine, const TIME c_max_star,
			    MACHINE* out_machine_index) {
	for (MACHINE j = 1; j <= num_machines; j++) {
		if (completion_time_per_machine[j] < c_max_star) {
			*out_machine_index = j;
			return 0;
		}
	}

	return 1;
}

int schedule_job(const MACHINE num_machines, const Operation* job, const TIME* completion_time_per_machine, double c_max_star,
	     int* out_is_preempted, MACHINE* out_machine_id, ScheduledJob* out_scheduled_job) {
	MACHINE machine_id;
	if (first_available_machine(num_machines, completion_time_per_machine, c_max_star, &machine_id)) {
		fprintf(stderr, "No available machine");
		return 1;
	}
	*out_machine_id = machine_id;

	// Create job
	TIME starting_time = completion_time_per_machine[machine_id];
	starting_time = job->prdwjm[R] > starting_time ? job->prdwjm[R] : starting_time;
	TIME end_time = starting_time + job->prdwjm[P];
	if ((double) end_time > c_max_star) {
		end_time = c_max_star;
		*out_is_preempted = 1;
	}
	*out_scheduled_job = (ScheduledJob) {
		.start = starting_time,
		.end = end_time,
		.job_j = (INDEX) job->prdwjm[J],
		.m_id = machine_id // single job can be on multiple machines in a preemptible schedule
	};
	return 0;
}

void max_time(TIME* prev, const TIME* cur) {
	if(cur < prev) {
		*prev = *cur;
	}
}

int mcnaughtons(const MACHINE num_machines, Input* inout_input, Schedule* inout_schedule) {
	int rv = 0;
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	TIME c_max_star_int;
	if (min_max_time(inout_input->operations, sizeof(Operation), inout_input->length, &c_max_star_int,
		     (MIN_MAX_CMP_FUNC) max_time)) {
		fprintf(stderr, "Invalid data");
		return 1;
	}
	double c_max_star = (double) c_max_star_int;

	TIME processing_sum;
	if (sum_time(inout_input->operations, sizeof(Operation), inout_input->length, (GET_INT) get_processing_time,
		     &processing_sum)) {
		fprintf(stderr, "Invalid data");
		return 1;
	}
	c_max_star = MAX(c_max_star, (double) processing_sum / (double) num_machines);

	unsigned int num_full_jobs_scheduled = 0;

	// Add one to be able to operate in 1..=num_machines range and not worry about error-prone -1 conversions
	TIME* completion_time_per_machine = calloc(num_machines + 1, sizeof(TIME));
	while (num_full_jobs_scheduled < inout_input->length) {
		Operation* job = &inout_input->operations[num_full_jobs_scheduled];

		int is_preempted = 0;
		MACHINE machine_id;
		ScheduledJob job_to_be_scheduled;
		if (schedule_job(num_machines, job, completion_time_per_machine,
				 c_max_star, &is_preempted, &machine_id, &job_to_be_scheduled)) {
			rv = 1;
			goto error_cleanup;
		}
		TIME length_of_this_part_of_job = (job_to_be_scheduled.end - job_to_be_scheduled.start);

		if (is_preempted) {
			if (inout_input->length + 1 > schedule_capacity) {
				schedule_capacity *= 2;
				if (!realloc(inout_schedule->schedule, schedule_capacity * sizeof(ScheduledJob))) {
					 perror("OOM");
					 rv = 1;
					 goto error_cleanup;
				}
			}
			job->prdwjm[P] -= length_of_this_part_of_job; // Now we want to schedule again what we cut off
		} else {
			// We move on to the next job
			num_full_jobs_scheduled++;
		}
		inout_schedule->schedule[inout_schedule->length++] = job_to_be_scheduled;

		completion_time_per_machine[machine_id] += length_of_this_part_of_job;
	}

error_cleanup:
	free(completion_time_per_machine); //FIXME: double free/corruption (linux only???)
	completion_time_per_machine = NULL;
	return rv;
}

int modified_spt(const MACHINE num_machines, Input* inout_input, Schedule* inout_schedule) {
	qsort(inout_input->operations, inout_input->length, sizeof(Operation), (COMPARE_FUNC) nondecreasing_processing_time);

	int rv = 0;
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	// Add one to be able to operate in 1..=num_machines range and not worry about error-prone -1 conversions
	TIME* completion_time_per_machine = calloc(num_machines + 1, sizeof(TIME));
	while (inout_schedule->length < inout_input->length) {
		unsigned int num_machines_or_less = MIN(num_machines, inout_input->length - inout_schedule->length);
		for(unsigned int i = 0; i < num_machines_or_less; i++) {
			Operation* job = &inout_input->operations[inout_schedule->length];
			MACHINE machine_id = i + 1;
			TIME start_time = completion_time_per_machine[machine_id];
			ScheduledJob job_to_be_scheduled = {
				.start = start_time,
				.end = start_time + job->prdwjm[P],
				.m_id = machine_id,
				.job_j = (INDEX) job->prdwjm[J]
			};
			TIME length_of_this_part_of_job = (job_to_be_scheduled.end - job_to_be_scheduled.start);
			inout_schedule->schedule[inout_schedule->length++] = job_to_be_scheduled;
			completion_time_per_machine[machine_id] += length_of_this_part_of_job;
		}
	}

	free(completion_time_per_machine);
	completion_time_per_machine = NULL;
	return rv;
}


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
int johnsons(Input* inout_input, Schedule* inout_schedule) {
	int rv = 0;
	// List operation processing times p1j and p2j in two columns
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	size_t num_jobs = inout_input->length / 2;
	// + 1, in case of odd number of jobs one of the indices in the middle will be 0
	size_t final_sequence_len = num_jobs + 1;
	INDEX* final_sequence = malloc(final_sequence_len * sizeof(INDEX));
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
		final_sequence[index_to_put_index_in] = (INDEX) min.operation.prdwjm[J];

		// 4. Delete both operation processing times from the columns
		min.map_index_to_whether_inserted[(INDEX)min.operation.prdwjm[J] - 1] = 1;
		min.operation.prdwjm[P] = TIME_MAX;
	}
	free(min.map_index_to_whether_inserted);

	// We get rid of the possible 0 in the middle in case of odd number of jobs
	memmove(
		final_sequence + cursor_end_of_1st_part,
		final_sequence + (final_sequence_len / 2),
		cursor_end_of_2nd_part - (final_sequence_len / 2)
	);
	final_sequence_len--;
	printf("Final sequence:\n");
	for(int i = 0; i < final_sequence_len; i++) {
		printf("%d, ", final_sequence[i]);
	}
	printf("\n");

	free(final_sequence);

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
int gonzalez_sahni(Input* inout_input, Schedule* inout_schedule) {
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
	INDEX* final_sequence_of_1st_m_1st_part = malloc(num_operations * sizeof(INDEX));
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
	INDEX* cursor = final_sequence_of_1st_m_1st_part;
	for(int m = 0; m < 2; m++) {
		printf("Sequence of jobs on machine %d\n", m + 1);
		for(int i = 0; i < num_jobs; i++) {
			INDEX j = *(cursor++);
			printf("%d, ", j);
		}
		printf("\n");
	}

	free(map_index_to_whether_job_belongs_to_first_set);
	free(final_sequence_of_1st_m_1st_part);

	return 0;
}

int jacksons(const MACHINE num_machines, Input* inout_input, Schedule* inout_schedule) {
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
	if (begins_with(mode, "john")) {
		if(num_machines == 3) {
			fprintf(stderr, "TODO: slide 14 lecture 10, special case for F3");
			exit(1);
		} else if(num_machines != 2) {
			fprintf(stderr, "Johnson's algorithm only works for 2 machines!");
			exit(1);
		}

		johnsons(in_input, out_schedule);
	} else if (begins_with(mode, "gon") || begins_with(mode, "br")){
		gonzalez_sahni(in_input, out_schedule);
	} else if (begins_with(mode, "jack")) {
		jacksons(num_machines, in_input, out_schedule);
	} else {
		fprintf(stderr, "Invalid mode %s\n", mode);
		return ERROR_WITH_HELP;
	}
	return rv;
}

