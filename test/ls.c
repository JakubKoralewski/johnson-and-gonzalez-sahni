#include "../src/imp2.c"
#include "assert.c"

int main() {
	int rv;
	Input input;
	Job jobs[3] = {
		{
			.j = 2,
			.prdw = {2, 4, 0, 0}
		},
		{
			.j = 1,
			.prdw = {1, 2, 0, 0},
		},
		{
			.j = 3,
			.prdw = {1, 5, 0, 0}
		}
	};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P | 1 << R;

	Schedule schedule;

	if ((rv = imp1(&schedule, &input, "erd"))) {
		assert(rv == 0, "%s", "imp1() call should succeed");
	}
	for (int i = 0; i < len; i++) {
		assert(schedule.input->buffer[i].j == i + 1, "%s (len %d) was %zu %zu %zu", "1,2,3 order", len,
		       input.buffer[0].j, input.buffer[1].j, input.buffer[2].j);
	}
	assert(schedule.schedule[0].start == 2, "%s", "ready time of first job");
	assert(schedule.schedule[0].end == 3, "%s", "processing time of 1st job");

	assert(schedule.schedule[1].start == 4, "%s", "ready time of 2nd job");
	assert(schedule.schedule[1].end == 6, "%s", "p of 2nd job");

	assert(schedule.schedule[2].start == 6, "%s", "end of 2nd job");
	assert(schedule.schedule[2].end == 7, "%s", "p t of 3rd job");
}
