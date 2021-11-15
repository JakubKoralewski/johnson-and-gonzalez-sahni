#include "../src/imp1.c"
#include "assert.c"

int main() {
	int rv;
	Input input;
	Job jobs[3] = {
		{
			.j = 1,
			.prdw = {2, 4, 0, 4} //p/w = 1/2
		},
		{
			.j = 2,
			.prdw = {20, 2, 0, 10}, //p/w = 2
		},
		{
			.j = 3,
			.prdw = {5, 5, 0, 5} //p/w = 1
		}
	};
	INDEX order[3] = {1,3,2};
	input.buffer = jobs;
	int len = sizeof(jobs) / sizeof(Job);
	input.length = len;
	input.which_set = 1 << P | 1 << R | 1 << W;

	Schedule schedule;

	if ((rv = imp1(&schedule, &input, "wspt"))) {
		assert(rv == 0, "%s", "imp1() call should succeed");
	}
	for (int i = 0; i < len; i++) {
		assert(schedule.input->buffer[i].j == order[i], "%s (len %d) was %zu %zu %zu", "3,2,1 order", len,
		       input.buffer[0].j, input.buffer[1].j, input.buffer[2].j);
	}
	assert_eq(schedule.schedule[0].start, 4, "%lu", "%d", "");
	assert_eq(schedule.schedule[0].end, 4 + 2, "%lu", "%d", "");

	assert_eq(schedule.schedule[1].start, 6, "%lu", "%d", "");
	assert_eq(schedule.schedule[1].end, 6 + 5, "%lu", "%d", "");

	assert_eq(schedule.schedule[2].start, 6 + 5, "%lu", "%d", "");
	assert_eq(schedule.schedule[2].end, 11 + 20, "%lu", "%d", "");
}
