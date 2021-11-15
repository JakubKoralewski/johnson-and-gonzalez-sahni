#include "../src/imp1.c"
#include "assert.c"

int main() {
	int rv;
	Input input;
	Job jobs[3] = {
		{
			.j = 1,
			.prdw = {2, 4, 10, 0}
		},
		{
			.j = 2,
			.prdw = {1, 2, 9, 0},
		},
		{
			.j = 3,
			.prdw = {1, 5, 8, 0}
		}
	};
	INDEX order[3] = {3,2,1};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P | 1 << R | 1 << D;

	Schedule schedule;

	if ((rv = imp1(&schedule, &input, "edd"))) {
		assert(rv == 0, "%s", "imp1() call should succeed");
	}
	for (int i = 0; i < len; i++) {
		assert(schedule.input->buffer[i].j == order[i], "%s (len %d) was %zu %zu %zu", "3,2,1 order", len,
		       input.buffer[0].j, input.buffer[1].j, input.buffer[2].j);
	}
	assert(schedule.schedule[0].start == 5, "");
	assert(schedule.schedule[0].end == 6, "");

	assert(schedule.schedule[1].start == 6, "");
	assert(schedule.schedule[1].end == 7, "");

	assert(schedule.schedule[2].start == 7, "");
	assert(schedule.schedule[2].end == 9, "");
}
