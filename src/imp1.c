// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
	#include <malloc.h>
	#define strcasecmp _stricmp
	#define getcwd _getcwd
#else
	#include <alloca.h>
	#include <strings.h>
	#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <limits.h>


#define TIME unsigned long
#define INDEX size_t

typedef struct {
	INDEX j;
	TIME prdw[4];
} Job;

#define P 0
#define R 1
#define D 2
#define W 3

// Collection of jobs, possibly unfeasible ordering
typedef struct {
	unsigned char which_set;
	size_t length;
	Job* buffer;
} Input;

typedef struct {
	TIME start;
	TIME end;
} ScheduledJob;

typedef struct {
	Input* input;
	// Length of the schedule is input->length
	ScheduledJob* schedule;
} Schedule;

void schedule_print(Schedule* schedule) {
	printf("Input:\n");
	for (size_t i = 0; i < schedule->input->length; i++) {
		Job* job = schedule->input->buffer + i;
		ScheduledJob* schedule_data = schedule->schedule + i;
		printf("Job #%zd: start: %lu; end: %lu\n", job->j, schedule_data->start, schedule_data->end);
	}
}
// num digits of MAX_ULONG_LEN 18446744073709551615
#define MAX_ULONG_LEN 20
//                       processing time ", " ready time     ", " deadline         + ", " + weight    \r \n  \0
#define LINE_BUFFER_SIZE (MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 1 + 1 + 1)
#define NO_PROPERTY 0xFF

//https://stackoverflow.com/questions/11793689/read-the-entire-contents-of-a-file-to-c-char-including-new-lines
int read_input_from_file(const char* path, Input* input) {
	size_t buffer_size = 8;
	input->buffer = malloc(buffer_size * sizeof(Job));

	char line_buffer[LINE_BUFFER_SIZE];
	size_t num_rows = 0;

	FILE* file = NULL;
	if (!(file = fopen(path, "rb"))) {
		char cwd[512];
		getcwd(cwd, sizeof(cwd));
		fprintf(stderr, "Couldn't open file '%s' in cwd '%s'; FILE: '%s'\n", path, cwd, __FILE__);
		goto error_cleanup;
	}
	// e.g. {P, R, NP,NP} corresponds to header p,r; 5th value is sentinel to stop loop over this array
	unsigned char order_prdw[5] = {NO_PROPERTY, NO_PROPERTY, NO_PROPERTY, NO_PROPERTY, NO_PROPERTY};
	// Read header row
	if (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		// Separate indices as we
		unsigned int char_index = 0;
		unsigned char property_index = 0;
		while (line_buffer[char_index] != '\0') {
			switch (line_buffer[char_index]) {
				case 'p':
					order_prdw[property_index] = P;
					break;
				case 'r':
					order_prdw[property_index] = R;
					break;
				case 'd':
					order_prdw[property_index] = D;
					break;
				case 'w':
					order_prdw[property_index] = W;
					break;
				case '\r':
				case '\n':
				case '\t':
				case ',':
					property_index--;
					break;
				default:
					goto no_header_row;
			}
			char_index++;
			property_index++;
		}
	} else {
		goto no_header_row;
	}
	for (int i = 0; i < 4; i++) {
		unsigned char pos = order_prdw[i];
		if (pos != NO_PROPERTY) {
			input->which_set |= 1 << pos;
		} else {
			break;
		}
	}
	// https://cplusplus.com/reference/cstdio/fgets/
	while (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		if (num_rows >= buffer_size) {
			buffer_size *= 2;
			if (!realloc(input->buffer, buffer_size)) {
				perror("OOM");
				goto error_cleanup;
			}
		}

		char* parse_next = line_buffer;
		char* endptr;
		Job job = {
			.j = num_rows + 1 // including header + 1
		};
		unsigned char* property = order_prdw;
		while (*property != NO_PROPERTY) {
			// We get property number, which corresponds to index to use for saving in prdw array
			errno = 0;
			TIME value = strtoul(parse_next, &endptr, 10);
			if (errno) {
				// Error in conversion
				goto error;
			} else if (*parse_next != '\0' && parse_next == endptr) {
				// Hasn't moved (invalid character)

				// Make sure it is not `inf`
				char* comma = strchr(parse_next, ',');
				if (comma != NULL) {
					*comma = '\0'; // Found comma, no need to restore it as we'll jump past it
				} else {
					parse_next[3] = '\0'; // only interested in 3 characters (gets rid of \r\n etc.)
				}
				if (strcasecmp(parse_next, "inf") == 0) {
					value = ULONG_MAX;
					endptr = comma; // so next time we start after the comma we found
					goto success;
				}

				goto error;
			}  else {
				goto success;
			}
error:
			fprintf(stderr, "Data file %s invalid on row (not including header) %zu\n", path, num_rows);
			goto error_cleanup;
success:
			job.prdw[*property] = value;
			parse_next = endptr + 1; // + 1 to get past the comma
			property++;
		}

		*(input->buffer + num_rows) = job;
		num_rows++;
	}
	if (num_rows == 0) {
		fprintf(stderr, "Not enough values in file\n");
		goto error_cleanup;
	}
	input->length = num_rows;
	fclose(file);
	return 0;

no_header_row:
	fprintf(stderr, "No header row in file: %s\n", path);
	// also needs error cleanup

error_cleanup:
	free(input->buffer);
	input->buffer = NULL;
	if (file != NULL) {
		fclose(file);
	}
	return 1;
}

//https://stackoverflow.com/questions/50559106/universal-array-element-swap-in-c
void swap(void* v1, void* v2, size_t size) {
	char* temp = (char*) alloca(size);
	memmove(temp, v1, size);
	memmove(v1, v2, size);
	memmove(v2, temp, size);
}

size_t index_parent(size_t child) {
	return (child - 1) / 2;
}

typedef const int (* COMPARE_FUNC)(void*, void*);

void sift_up(void* buffer, size_t start, size_t end, size_t size, COMPARE_FUNC cmp) {
	size_t child = end;
	while (child > start) {
		size_t parent = index_parent(child);
		void* parent_pos = (void*) ((char*) buffer + parent * size);
		void* child_pos = (void*) ((char*) buffer + child * size);
		if (cmp(parent_pos, child_pos)) {
			swap(parent_pos, child_pos, size);
			child = parent;
		} else {
			return;
		}
	}
}

// https://en.wikipedia.org/wiki/Heapsort
void heapify(void* buffer, size_t length, size_t size, COMPARE_FUNC cmp) {
	// Index of first child of root
	size_t end = 1;

	while (end < length) {
		sift_up(buffer, 0, end, size, cmp);
		end++;
	}
}


void heapsort(void* buffer, size_t length, size_t size, COMPARE_FUNC cmp) {
	heapify(buffer, length, size, cmp);
	size_t end = length - 1;
	while (end > 0) {
		swap((void*) ((char*) buffer + end * size), (void*) buffer, size);
		heapify(buffer, end, size, cmp);
		end--;
	}
}

int min_ready_time(Job* a, Job* b) {
	return a->prdw[R] < b->prdw[R];
}

int min_deadline(Job* a, Job* b) {
	return a->prdw[D] < b->prdw[D];
}

int min_processing_time(Job* a, Job* b) {
	return a->prdw[P] < b->prdw[P];
}

int min_weighted_processing_time(Job* a, Job* b) {
	return (double) a->prdw[P] / (double) a->prdw[W] < (double) b->prdw[P] / (double) b->prdw[W];
}

#define ERROR_WITH_HELP 789

void schedule_new(Schedule* inout_schedule, Input* in_input) {
	inout_schedule->input = in_input;
	inout_schedule->schedule = malloc(in_input->length * sizeof(Job));
	TIME starting_time = 0;
	for (int i = 0; i < in_input->length; i++) {
		Job* job = &in_input->buffer[i];
		starting_time = job->prdw[R] > starting_time ? job->prdw[R] : starting_time;
		ScheduledJob schedule_data = {
			.start = starting_time,
			.end = starting_time + job->prdw[P]
		};
		starting_time = schedule_data.end;
		inout_schedule->schedule[i] = schedule_data;
	}
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
	heapsort(in_input->buffer, in_input->length, sizeof(Job), cmp);
	schedule_new(out_schedule, in_input);
	return rv;
}

