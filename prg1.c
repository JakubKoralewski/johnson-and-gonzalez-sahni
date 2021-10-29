// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <malloc.h>
#ifndef _WIN32
	#include <alloca.h>
#endif
#include <string.h>
#include <strings.h>
#include <limits.h>

#define TIME unsigned int
#define INDEX unsigned int
#if _WIN32
	#define strcasecmp stricmp
#endif

typedef struct {
	INDEX j;
	TIME p;
	TIME r;
	TIME d;
} Job;

typedef struct {
	size_t length;
	Job* buffer;
} Input;

void input_drop(Input* input) {
	free(input->buffer);
}

void input_print(Input* input) {
	TIME s_j = 0;
	printf("Schedule:\n");
	for (size_t i = 0; i < input->length; i++) {
		Job* job = input->buffer + i;
		s_j = job->r > s_j ? job->r : s_j;
		printf("Job #%d: s_j: %d\n", job->j, s_j);
		s_j += job->p;
	}
}

// num digits of MAX_UINT 4294967295
#define MAX_UINT_LEN 10
//                       ready time     ", " processing t ", " deadline       \r \n  \0
#define LINE_BUFFER_SIZE (MAX_UINT_LEN + 2 + MAX_UINT_LEN + 2 + MAX_UINT_LEN + 1 + 1 + 1)

//https://stackoverflow.com/questions/11793689/read-the-entire-contents-of-a-file-to-c-char-including-new-lines
int read_input_from_file(const char* path, Input* input) {
	size_t buffer_size = 8;
	input->buffer = malloc(buffer_size * sizeof(Job));

	char line_buffer[LINE_BUFFER_SIZE];
	size_t num_rows = 0;

	FILE* file;
	if (!(file = fopen(path, "rb"))) {
		perror("Couldn't open file");
		return 1;
	}
	// https://cplusplus.com/reference/cstdio/fgets/
	while (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		if (num_rows >= buffer_size) {
			buffer_size *= 2;
			if (!realloc(input->buffer, buffer_size)) {
				perror("Ran out of memory");
				goto clean_up;
			}
		}

		char* endptr;
		errno = 0;
		// https://en.cppreference.com/w/c/string/byte/strtoimax
		const TIME p = strtoumax(line_buffer, &endptr, 10);
		endptr++;
		const TIME r = strtoumax(endptr, &endptr, 10);
		endptr++;
		if (errno) {
			perror("Invalid data");
			fprintf(stderr, "on row %zu\n", num_rows);
			goto clean_up;
		};
		TIME d = strtoumax(endptr, &endptr, 10);

		// Make sure that the `\r\n`/`\n` characters are not included in comparison
		*(endptr+3) = '\0';

		if (!d && strcasecmp(endptr, "inf") == 0) {
			d = UINT_MAX;
		}
		*(input->buffer + num_rows) = (Job) {
			.j = num_rows,
			.p = p,
			.r = r,
			.d = d
		};

		num_rows++;
	}
	input->length = num_rows;
	fclose(file);
	return 0;

clean_up:
	fclose(file);
	input_drop(input);
	return 1;
}
//typedef struct {
//	// Time at which the job should start
//	unsigned int start_time;
//} ScheduledJob;
//
//typedef struct {
//	ScheduledJob* jobs;
//} Schedule;
//void schedule_new(Schedule* schedule, size_t num_jobs) {
//	schedule->jobs = malloc(num_jobs * sizeof(ScheduledJob));
//}
//void schedule_drop(Schedule* schedule) {
//	free(schedule->jobs);
//}

//https://stackoverflow.com/questions/50559106/universal-array-element-swap-in-c
void swap(void* v1, void* v2, size_t size)
{
	char* temp = (char*)alloca(size);
	memmove(temp, v1, size);
	memmove(v1, v2, size);
	memmove(v2, temp, size);
}

size_t index_parent(size_t child) {
	return (child - 1) / 2;
}

void sift_up(void* buffer, size_t start, size_t end, size_t size, const int (*cmp)(void*, void*)) {
	size_t child = end;
	while (child > start) {
		size_t parent = index_parent(child);
		void* parent_pos = (void*) ((char*) buffer + parent * size);
		void* child_pos = (void*) ((char*) buffer + child * size);
		if (!cmp(parent_pos, child_pos)) {
			swap(parent_pos, child_pos, size);
			child = parent;
		} else {
			return;
		}
	}
}

// https://en.wikipedia.org/wiki/Heapsort
void heapify(void* buffer, size_t length, size_t size, const int (*cmp)(void*, void*)) {
	// Index of first child of root
	size_t end = 1;

	while (end < length) {
		sift_up(buffer, 0, end, size, cmp);
		end++;
	}
}

int min_ready_time(Job* a, Job* b) {
	return a->r < b->r;
}

void heapsort(void* buffer, size_t length, size_t size, const int (*cmp)(void*, void*)) {
	heapify(buffer, length, size, (const int (*)(void*, void*)) min_ready_time);
	size_t end = length - 1;
	while(end > 0) {
		swap((void*) ((char*)buffer + end * size), (void*) buffer, size);
		heapify(buffer, length, size, (const int (*)(void*, void*)) min_ready_time);
		end--;
	}
}

// Earliest Ready Time
void prg1(Input* input) {
	heapsort(input->buffer, input->length, sizeof(Job), (const int (*)(void*, void*)) min_ready_time);

	input_print(input);
}

int main(int argc, char** argv)
{
	char* path = argv[1];
	if (!path) {
		perror("No path given");
		return 1;
	}
	Input input;
	if (read_input_from_file(argv[1], &input)) {
		fprintf(stderr, "Couldn't read file %s", argv[1]);
		return 1;
	}
	prg1(&input);

	input_drop(&input);
	return 0;
}
