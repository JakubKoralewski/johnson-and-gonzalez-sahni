#include "../src/imp2.c"
#include "assert.c"

int main() {
	int rv;
	Input input;
	Job jobs[3] = {
		{
			.j = 1,
			.prdw = {2, 0, 0, 0}
		},
		{
			.j = 2,
			.prdw = {1, 0, 0, 0},
		},
		{
			.j = 3,
			.prdw = {4, 0, 0, 0}
		}
	};
	INDEX machine_assignments[] = {2,2,1};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P;

	Schedule schedule;
	MACHINE num_machines = 2;

	if ((rv = imp2(num_machines, &schedule, &input, "lpt"))) {
		assert(rv == 0, "%s", "imp2() call should succeed");
	}
	for (int i = 0; i < len; i++) {
		INDEX index_of_job = schedule.input->buffer[i].j;
		MACHINE m_id = machine_assignments[index_of_job-1];
		assert_eq(schedule.input->buffer[i].m_id, m_id, "%u", "%u", "i: %d", i);
	}
	assert(schedule.schedule[0].start == 0, "");
	assert(schedule.schedule[0].end == 4, "");

	assert(schedule.schedule[1].start == 0, "");
	assert(schedule.schedule[1].end == 2, "");

	assert(schedule.schedule[2].start == 2, "");
	assert(schedule.schedule[2].end == 3, "");
}
