#include "../src/imp3.c"
#include "assert.c"

int spt() {
	int rv;
	Input input;
	// https://i.imgur.com/loiEDCb.png
	Job jobs[] = { // processing times need to be distinct in case sorting is not stable for determinism
		{
			.j = 1,
			.prdw = {5}, // 4, m 1
		},
		{
			.j = 2,
			.prdw = {6} // 5, m 2
		},
		{
			.j = 3,
			.prdw = {3} // 2, m 2
		},
		{
			.j = 4,
			.prdw = {2} // 1, m 1
		},
		{
			.j = 5,
			.prdw = {4} // 3, m 3
		},
	};
	INDEX machine_assignments[] = {1,2,3,1,2};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P;
	MACHINE num_machines = 3;

	Schedule schedule;

	if ((rv = imp3(num_machines, &schedule, &input, "spt"))) {
		assert(rv == 0, "%s", "imp3() call should succeed");
	}
	for (unsigned int i = 0; i < schedule.length; i++) {
		ScheduledJob* scheduled_job = &schedule.schedule[i];
		MACHINE m_id = machine_assignments[i];
		assert_eq(scheduled_job->m_id, m_id, "%u", "%u", "i: %d", i);
	}
	assert(schedule.schedule[0].start == 0, "%s", "ready time of first job");
	assert(schedule.schedule[0].end == 2, "%s", "processing time of 1st job");

	assert(schedule.schedule[1].start == 0, "%s", "ready time of 2nd job");
	assert(schedule.schedule[1].end == 3, "%s", "p of 2nd job");

	assert(schedule.schedule[2].start == 0, "%s", "start 3rd job");
	assert(schedule.schedule[2].end == 4, "%s", "p t of 3rd job");

	assert(schedule.schedule[3].start == 2, "%s", "end of 2nd job");
	assert(schedule.schedule[3].end == 7, "%s", "p t of 3rd job");

	assert(schedule.schedule[4].start == 3, "%s", "end of 2nd job");
	assert(schedule.schedule[4].end == 9, "%s", "p t of 3rd job");

	return 0;
}
int main() {
	spt();
}
