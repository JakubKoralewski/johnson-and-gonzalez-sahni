// Jakub Koralewski 452490
// ERT, EDD, SPT, WSPT rules (PRG1, PRG2, PRG3 and PRG4, 6 points)
#include <stdio.h>
#include <errno.h>
#ifdef _WIN32
	#include <malloc.h>
	#define strcasecmp _stricmp
#else
	#include <alloca.h>
	#include <strings.h>
#endif
#include <string.h>
#include <stdlib.h>

#define TIME unsigned long
#define INDEX unsigned int

typedef struct {
	INDEX j;
	TIME prdw[4];
	unsigned char which_set;
} Job;

#define P 0
#define R 1
#define D 2
#define W 3

typedef struct {
	size_t length;
	Job* buffer;
} Input;

void input_print(Input* input) {
	TIME s_j = 0;
	printf("Schedule:\n");
	for (size_t i = 0; i < input->length; i++) {
		Job* job = input->buffer + i;
		s_j = job->prdw[R] > s_j ? job->prdw[R] : s_j;
		printf("Job #%d: s_j: %d\n", job->j, s_j);
		s_j += job->prdw[P];
	}
}
// num digits of MAX_ULONG_LEN 18446744073709551615
#define MAX_ULONG_LEN 20
//                       processing time ", " ready time     ", " deadline         + ", " + weight    \r \n  \0
#define LINE_BUFFER_SIZE (MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 2 + MAX_ULONG_LEN + 1 + 1 + 1)


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
	unsigned char order_prdw[4] = {0,0,0,0};
	// Read first line
	if (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		unsigned int char_index = 0;
		unsigned char property_index = 0;
		while(line_buffer[char_index] != '\0') {
			switch (line_buffer[char_index]) {
				case 'p': order_prdw[property_index] = P+1; break;
				case 'r': order_prdw[property_index] = R+1; break;
				case 'd': order_prdw[property_index] = D+1; break;
				case 'w': order_prdw[property_index] = W+1; break;
				case '\r': case '\n': case '\t':
				case ',': property_index--; break;
				default:
					fprintf(stderr, "No header row in file: %s", path);
					goto error_cleanup;
			}
			char_index++;
			property_index++;
		}
	}
	// https://cplusplus.com/reference/cstdio/fgets/
	while (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		if (num_rows >= buffer_size) {
			buffer_size *= 2;
			if (!realloc(input->buffer, buffer_size)) {
				perror("Ran out of memory");
				goto error_cleanup;
			}
		}

		char* endptr = line_buffer;
		Job job = {
			.which_set = 0,
			.j = num_rows + 1 // including header + 1
		};
		unsigned char* property = order_prdw;
		while(*property != 0) {
			errno = 0;
			job.which_set |= (1 << (*property - 1));
			const TIME value = strtoul(endptr, &endptr, 10);
			job.prdw[*property - 1] = value;
			endptr++;
			property++;
			if (errno) {
				perror("Invalid data file");
				fprintf(stderr, "Data file %s invalid on row %zu\n", path, num_rows);
				goto error_cleanup;
			};
		}

		*(input->buffer + num_rows) = job;
		num_rows++;
	}
	if(num_rows == 1) {
		fprintf(stderr, "Only header supplied, need actual values");
		goto error_cleanup;
	}
	input->length = num_rows;
	fclose(file);
	return 0;

error_cleanup:
	free(input->buffer);
	fclose(file);
	return 1;
}

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

typedef const int (*COMPARE_FUNC)(void*, void*);

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
	while(end > 0) {
		swap((void*) ((char*)buffer + end * size), (void*) buffer, size);
		heapify(buffer, end, size, cmp);
		end--;
	}
}

void print_help_menu() {
	printf(
		"\nThis is the help menu.\n"
		"Implementation 1 program by Jakub Koralewski.\n"
		"Example usages:\n"
		"\t- \"imp1.exe erd ./input.txt\"\n"
		"\t\t deadlines and weights are completely ignored for ERD\n"
		"\t- \".\\imp1.exe EDD .\\input.txt\"\n"
		"\t\t weights are completely ignored for EDD\n"
		"\t- \"imp1.exe spt input.txt\"\n"
		"\t\t deadlines and weights are completely ignored for SPT\n"
		"\t- \"imp1.exe WsPT input.txt\"\n"
		"\t\t deadlines are completely ignored for WSPT\n\n"
		"Where the contents of the input file are a subset of the CSV format defined as follows:\n"
		"\tThe first line is the header file. The allowed values are: 'p','r','d','w' which respectively correspond to:\n"
		"\t\t- processing time\n"
		"\t\t- ready time\n"
		"\t\t- deadline /due-date\n"
		"\t\t- weight\n"
		"\tOn each line put the same number of values separated by commas. Each column corresponding to the defined header.\n"
		"\tOnly non-negative integers are allowed (inc. weight). With the following exceptions:\n"
		"\t\tGive \"inf\" in place of deadline value to signify lack of deadline for given job. (Corresponds to maximal value fitting in given integer)\n"
		"\tJobs are numbered in the output based on the line on which they were defined in the file.\n"
		"\tExamples:"
		"\n"
		"\t\tp,r,d\n"
		"\t\t1,2,inf\n"
		"\t\t2,3,inf\n"
		"\t\t1,4,inf\n"
		"\n"
		"\t\tp,r,w\n"
		"\t\t1,2,2\n"
		"\t\t2,3,3\n"
		"\t\t1,4,1\n"
	);
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
	return (double)a->prdw[P]/(double)a->prdw[W] < (double)b->prdw[P]/(double)b->prdw[W];
}

#define ERROR_WITH_HELP 789

int imp1(Input* input, const char*mode, const char* path) {
	int rv = 0;
	if (!path) {
		fprintf(stderr, "No path given.");
		return ERROR_WITH_HELP;
	}
	if ((rv = read_input_from_file(path, input))) {
		fprintf(stderr, "Couldn't read file %s\n", path);
		return 1;
	}
	COMPARE_FUNC cmp;
	if(strcasecmp(mode, "erd") == 0) {
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
	heapsort(input->buffer, input->length, sizeof(Job), cmp);
	return rv;
}

int main(int argc, char** argv) {
	int rv = 0;
	if(argc != 3) {
		fprintf(stderr, "Invalid number of arguments.");
		rv = 123;
		goto error_with_help;
	}
	char* path = argv[2];
	char* mode = argv[1];
	Input input;
	switch((rv = imp1(&input, mode, path))) {
		case 0:
			input_print(&input);
			return rv;
		case ERROR_WITH_HELP:
			// Error that can be fixed by user learning about how to use this program
			goto error_with_help;
		default:
			// Any other error
			return rv;
	}

error_with_help:
	printf("\nSee help below:\n");
	print_help_menu();
	return rv;
}
