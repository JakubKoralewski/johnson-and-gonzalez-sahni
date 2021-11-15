// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include "schedule.c"

int min_ready_time(Job* a, Job* b) {
	return a->prdw[R] > b->prdw[R];
}

int min_deadline(Job* a, Job* b) {
	return a->prdw[D] > b->prdw[D];
}

int min_processing_time(Job* a, Job* b) {
	return a->prdw[P] > b->prdw[P];
}

int min_weighted_processing_time(Job* a, Job* b) {
	return (double) a->prdw[P] / (double) a->prdw[W] > (double) b->prdw[P] / (double) b->prdw[W];
}

int imp1(Schedule* out_schedule, Input* in_input, const char* mode) {
	int rv = 0;
	COMPARE_FUNC cmp;
	if (strcasecmp(mode, "erd") == 0) {
		cmp = (COMPARE_FUNC) min_ready_time;
	} else if (strcasecmp(mode, "edd") == 0) {
		cmp = (COMPARE_FUNC) min_deadline;
	} else if (strcasecmp(mode, "spt") == 0) {
		cmp = (COMPARE_FUNC) min_processing_time;
	} else if (strcasecmp(mode, "wspt") == 0) {
		cmp = (COMPARE_FUNC) min_weighted_processing_time;
	} else {
		fprintf(stderr, "Invalid mode %s\n", mode);
		return ERROR_WITH_HELP;
	}
	qsort(in_input->buffer, in_input->length, sizeof(Job), cmp);
	schedule_new(out_schedule, in_input);
	return rv;
}

