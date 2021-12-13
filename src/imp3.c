// Jakub Koralewski 452490
// McNaughton and modified SPT
#include "schedule.c"

int nonincreasing_processing_time(Job* a, Job* b) {
	return a->prdw[P] < b->prdw[P];
}

int min_processing_time(Job* a, Job* b) {
	return a->prdw[P] > b->prdw[P];
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

TIME get_processing_time(Job* job) {
	return job->prdw[P];
}

int
schedule_job(const MACHINE num_machines, const Job* job, const TIME* completion_time_per_machine, double c_max_star,
	     int* out_is_preempted, MACHINE* out_machine_id, ScheduledJob* out_scheduled_job) {
	MACHINE machine_id;
	if (first_available_machine(num_machines, completion_time_per_machine, c_max_star, &machine_id)) {
		fprintf(stderr, "No available machine");
		return 1;
	}
	*out_machine_id = machine_id;

	// Create job
	TIME starting_time = completion_time_per_machine[machine_id];
	starting_time = job->prdw[R] > starting_time ? job->prdw[R] : starting_time;
	TIME end_time = starting_time + job->prdw[P];
	if ((double) end_time > c_max_star) {
		end_time = c_max_star;
		*out_is_preempted = 1;
	}
	*out_scheduled_job = (ScheduledJob) {
		.start = starting_time,
		.end = end_time,
		.job_j = job->j,
		.m_id = machine_id // single job can be on multiple machines in a preemptible schedule
	};
	return 0;
}

int mcnaughtons(const MACHINE num_machines, Input* inout_input, Schedule* inout_schedule) {
	int rv = 0;
	inout_schedule->input = inout_input;
	inout_schedule->length = 0;
	size_t schedule_capacity = inout_input->length;
	inout_schedule->schedule = malloc(schedule_capacity * sizeof(ScheduledJob));

	TIME c_max_star_int;
	if (max_time(inout_input->buffer, sizeof(Job), inout_input->length, (GET_INT) get_processing_time,
		&c_max_star_int)) {
		fprintf(stderr, "Invalid data");
		return 1;
	}
	double c_max_star = (double) c_max_star_int;

	TIME processing_sum;
	if (sum_time(inout_input->buffer, sizeof(Job), inout_input->length, (GET_INT) get_processing_time,
		&processing_sum)) {
		fprintf(stderr, "Invalid data");
		return 1;
	}
	c_max_star = MAX(c_max_star, (double) processing_sum / (double) num_machines);

	unsigned int num_full_jobs_scheduled = 0;

	// Add one to be able to operate in 1..=num_machines range and not worry about error-prone -1 conversions
	TIME* completion_time_per_machine = calloc(num_machines + 1, sizeof(TIME));
	while (num_full_jobs_scheduled < inout_input->length) {
		Job* job = &inout_input->buffer[num_full_jobs_scheduled];

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
			job->prdw[P] -= length_of_this_part_of_job; // Now we want to schedule again what we cut off
		} else {
			// We move on to the next job
			num_full_jobs_scheduled++;
		}
		inout_schedule->schedule[inout_schedule->length++] = job_to_be_scheduled;

		completion_time_per_machine[machine_id] += length_of_this_part_of_job;
	}

error_cleanup:
	free(completion_time_per_machine); //FIXME: double free/corruption
	completion_time_per_machine = NULL;
	return rv;
}

int imp3(MACHINE num_machines, Schedule* out_schedule, Input* in_input, const char* mode) {
	int rv = 0;
	if (strcasecmp(mode, "mcn") == 0 || strcasecmp(mode, "mcnaughton") == 0) {
		if (require_set(in_input, 1 << P)) {
			fprintf(stderr, "McNaughton's requires processing times!");
			exit(1);
		}
		mcnaughtons(num_machines, in_input, out_schedule);
	} else if (strcasecmp(mode, "spt") == 0) {
		// todo
		exit(1);
		if (require_set(in_input, 1 << P)) {
			fprintf(stderr, "Modified SPT requires processing times!");
			exit(1);
		}
		qsort(in_input->buffer, in_input->length, sizeof(Job), (COMPARE_FUNC) nonincreasing_processing_time);
//		list_scheduling(num_machines, in_input, out_schedule);
	} else {
		fprintf(stderr, "Invalid mode %s\n", mode);
		return ERROR_WITH_HELP;
	}
	return rv;
}

