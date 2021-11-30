#include "../src/imp2.c"
#include "assert.c"

int main() {
	int rv;
	Input input;
	Job jobs[3] = {
		{
			.j = 1,
			.prdw = {1, 0, 0, 0},
		},
		{
			.j = 2,
			.prdw = {2, 0, 0, 0}
		},
		{
			.j = 3,
			.prdw = {1, 0, 0, 0}
		}
	};
	INDEX machine_assignments[] = {1,2,1};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 2;

	Schedule schedule;

	if ((rv = imp2(num_machines, &schedule, &input, "ls"))) {
		assert(rv == 0, "%s", "imp2() call should succeed");
	}
	for (int i = 0; i < len; i++) {
		INDEX index_of_job = schedule.input->buffer[i].j;
		MACHINE m_id = machine_assignments[index_of_job-1];
		assert_eq(schedule.input->buffer[i].m_id, m_id, "%u", "%u", "i: %d", i);
	}
	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
	assert(schedule.schedule[0].end == 1, "%s", "processing time of 1st job");

	assert(schedule.schedule[1].start == 0, "%s", "ready time of 2nd job");
	assert(schedule.schedule[1].end == 2, "%s", "p of 2nd job");

	assert(schedule.schedule[2].start == 1, "%s", "end of 2nd job");
	assert(schedule.schedule[2].end == 2, "%s", "p t of 3rd job");
}
