#include "../src/imp4.c"
#include "assert.c"

int example_odd_all_in_1st_part() {
	int rv;
	Input input;
	// see johnsons_all_in_1st_part.txt for more friendly view at this data
	Operation operations[] = {
		{
			.prdwjm = {2, 0, 0, 0, 1, 1},
		},
		{
			.prdwjm = {3, 0, 0, 0, 1, 2},
		},
		{
			.prdwjm = {1, 0, 0, 0, 2, 1},
		},
		{
			.prdwjm = {3, 0, 0, 0, 2, 2},
		},
		{
			.prdwjm = {2, 0, 0, 0, 3, 1},
		},
		{
			.prdwjm = {3, 0, 0, 0, 3, 2},
		},
	};
	INDEX machine_assignments[] = {1,2,1,2,1,2};
	input.operations = operations;
	int len = sizeof(operations) / sizeof(Operation);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 2;

	Schedule schedule;

	if ((rv = imp4(num_machines, &schedule, &input, "gon"))) {
		assert(rv == 0, "%s", "imp4() call should succeed");
	}
//	for (unsigned int i = 0; i < schedule.length; i++) {
//		ScheduledJob* scheduled_job = &schedule.schedule[i];
//		MACHINE m_id = machine_assignments[i];
//		assert_eq(scheduled_job->m_id, m_id, "%u", "%u", "i: %d", i);
//	}
//	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
//	assert(schedule.schedule[0].end == 2, "%s", "processing time of 1st job");
//
//	assert(schedule.schedule[1].start == 2, "%s", "ready time of 2nd job");
//	assert(schedule.schedule[1].end == 5, "%s", "p of 2nd job");
//
//	assert(schedule.schedule[2].start == 5, "%s", "start 3rd job");
//	assert(schedule.schedule[2].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[3].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[3].end == 3, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[4].start == 3, "%s", "end of 2nd job");
//	assert(schedule.schedule[4].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[5].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[5].end == 3, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[6].start == 3, "%s", "end of 2nd job");
//	assert(schedule.schedule[6].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[7].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[7].end == 3, "%s", "p t of 3rd job");

	return 0;
}

int example_odd_both_parts() {
	int rv;
	Input input;
	// see johnsons_both_parts.txt for more friendly view at this data
	Operation operations[] = {
		{
			.prdwjm = {2, 0, 0, 0, 1, 1},
		},
		{
			.prdwjm = {3, 0, 0, 0, 1, 2},
		},
		{
			.prdwjm = {1, 0, 0, 0, 2, 1},
		},
		{
			.prdwjm = {1, 0, 0, 0, 2, 2},
		},
		{
			.prdwjm = {2, 0, 0, 0, 3, 1},
		},
		{
			.prdwjm = {3, 0, 0, 0, 3, 2},
		},
	};
	INDEX machine_assignments[] = {1,2,1,2,1,2};
	input.operations = operations;
	int len = sizeof(operations) / sizeof(Operation);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 2;

	Schedule schedule;

	if ((rv = imp4(num_machines, &schedule, &input, "gon"))) {
		assert(rv == 0, "%s", "imp4() call should succeed");
	}
	for (unsigned int i = 0; i < schedule.length; i++) {
		ScheduledJob* scheduled_job = &schedule.schedule[i];
		MACHINE m_id = machine_assignments[i];
		assert_eq(scheduled_job->m_id, m_id, "%u", "%u", "i: %d", i);
	}
//	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
//	assert(schedule.schedule[0].end == 2, "%s", "processing time of 1st job");
//
//	assert(schedule.schedule[1].start == 2, "%s", "ready time of 2nd job");
//	assert(schedule.schedule[1].end == 5, "%s", "p of 2nd job");
//
//	assert(schedule.schedule[2].start == 5, "%s", "start 3rd job");
//	assert(schedule.schedule[2].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[3].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[3].end == 3, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[4].start == 3, "%s", "end of 2nd job");
//	assert(schedule.schedule[4].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[5].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[5].end == 3, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[6].start == 3, "%s", "end of 2nd job");
//	assert(schedule.schedule[6].end == 7, "%s", "p t of 3rd job");
//
//	assert(schedule.schedule[7].start == 0, "%s", "end of 2nd job");
//	assert(schedule.schedule[7].end == 3, "%s", "p t of 3rd job");

	return 0;
}
int main() {
//	example_odd_all_in_1st_part();
	example_odd_both_parts();
}
