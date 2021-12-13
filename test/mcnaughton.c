#include "../src/imp3.c"
#include "assert.c"

int slide29_example1() {
	int rv;
	Input input;
	// Slide 29 lect no. 6
	Job jobs[] = {
		{
			.j = 1,
			.prdw = {2, 0, 0, 0},
		},
		{
			.j = 2,
			.prdw = {3, 0, 0, 0}
		},
		{
			.j = 3,
			.prdw = {5, 0, 0, 0}
		},
		{
			.j = 4,
			.prdw = {7, 0, 0, 0}
		},
		{
			.j = 5,
			.prdw = {4, 0, 0, 0}
		},
		{
			.j = 6,
			.prdw = {3,0, 0, 0}
		},
	};
	INDEX machine_assignments[] = {1,1,1,2,2,3,3,4};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 4;

	Schedule schedule;

	if ((rv = imp3(num_machines, &schedule, &input, "mcn"))) {
		assert(rv == 0, "%s", "imp3() call should succeed");
	}
	for (unsigned int i = 0; i < schedule.length; i++) {
		ScheduledJob* scheduled_job = &schedule.schedule[i];
		MACHINE m_id = machine_assignments[i];
		assert_eq(scheduled_job->m_id, m_id, "%u", "%u", "i: %d", i);
	}
	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
	assert(schedule.schedule[0].end == 2, "%s", "processing time of 1st job");

	assert(schedule.schedule[1].start == 2, "%s", "ready time of 2nd job");
	assert(schedule.schedule[1].end == 5, "%s", "p of 2nd job");

	assert(schedule.schedule[2].start == 5, "%s", "start 3rd job");
	assert(schedule.schedule[2].end == 7, "%s", "p t of 3rd job");

	assert(schedule.schedule[3].start == 0, "%s", "end of 2nd job");
	assert(schedule.schedule[3].end == 3, "%s", "p t of 3rd job");

	assert(schedule.schedule[4].start == 3, "%s", "end of 2nd job");
	assert(schedule.schedule[4].end == 7, "%s", "p t of 3rd job");

	assert(schedule.schedule[5].start == 0, "%s", "end of 2nd job");
	assert(schedule.schedule[5].end == 3, "%s", "p t of 3rd job");

	assert(schedule.schedule[6].start == 3, "%s", "end of 2nd job");
	assert(schedule.schedule[6].end == 7, "%s", "p t of 3rd job");

	assert(schedule.schedule[7].start == 0, "%s", "end of 2nd job");
	assert(schedule.schedule[7].end == 3, "%s", "p t of 3rd job");

	return 0;
}

int slide30_example2() {
	int rv;
	Input input;
	// Slide 29 lect no. 6
	Job jobs[] = {
		{
			.j = 1,
			.prdw = {4},
		},
		{
			.j = 2,
			.prdw = {5}
		},
		{
			.j = 3,
			.prdw = {3}
		},
		{
			.j = 4,
			.prdw = {5}
		},
		{
			.j = 5,
			.prdw = {4}
		},
	};
	INDEX machine_assignments[] = {1,1,2,2,2,3,3};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 3;

	Schedule schedule;

	if ((rv = imp3(num_machines, &schedule, &input, "mcn"))) {
		assert(rv == 0, "%s", "imp3() call should succeed");
	}
	for (unsigned int i = 0; i < schedule.length; i++) {
		ScheduledJob* scheduled_job = &schedule.schedule[i];
		MACHINE m_id = machine_assignments[i];
		assert_eq(scheduled_job->m_id, m_id, "%u", "%u", "i: %d", i);
	}
	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
	assert(schedule.schedule[0].end == 4, "%s", "processing time of 1st job");

	assert(schedule.schedule[1].start == 4, "%s", "ready time of 2nd job");
	assert(schedule.schedule[1].end == 7, "%s", "p of 2nd job");

	assert(schedule.schedule[2].start == 0, "%s", "start 3rd job");
	assert(schedule.schedule[2].end == 2, "%s", "p t of 3rd job");

	assert(schedule.schedule[3].start == 2, "%s", "end of 2nd job");
	assert(schedule.schedule[3].end == 5, "%s", "p t of 3rd job");

	assert(schedule.schedule[4].start == 5, "%s", "end of 2nd job");
	assert(schedule.schedule[4].end == 7, "%s", "p t of 3rd job");

	assert(schedule.schedule[5].start == 0, "%s", "end of 2nd job");
	assert(schedule.schedule[5].end == 3, "%s", "p t of 3rd job");

	assert(schedule.schedule[6].start == 3, "%s", "end of 2nd job");
	assert(schedule.schedule[6].end == 7, "%s", "p t of 3rd job")

	return 0;
}

int main() {
	slide29_example1();
	slide30_example2();
}
