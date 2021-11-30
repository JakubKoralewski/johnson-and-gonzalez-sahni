#include "../src/imp2.c"
#include "assert.c"
#include <string.h>

//#ifdef _WIN32
//	#define SLASH "\\"
//#else
	#define SLASH "/"
//#endif

#define TEST_DIR_MAX_SIZE 1024


// Assumes the path already has the TEST_DIR as the beginning
int make_file_path_relative_to_src_file(const char* relative_path_to_input, char (* path)[TEST_DIR_MAX_SIZE]) {
	fprintf(stderr, "TEST_DIR: " TEST_DIR "\n");
	const size_t test_dir_len = strlen(TEST_DIR);
	if(test_dir_len >= TEST_DIR_MAX_SIZE) {
		fprintf(stderr, "TEST_DIR too large (%zu)", test_dir_len);
		return 1;
	}
	// Append the path to input to test directory
	memcpy(*path + test_dir_len, relative_path_to_input, strlen(relative_path_to_input));
	// End string
	unsigned short new_len = test_dir_len + strlen(relative_path_to_input);
	(*path)[new_len] = '\0';

	return 0;
}

void check_deadline_of_first_row_is_infinity(Input* input) {
	// Assumes wrap-around arithmetic
	assert_eq(input->buffer[0].prdw[D] + 1, 0, "%lu", "%d", "Inf is actually inf");
}
#define I(a) "inputs" SLASH a
int main() {
	Input input;
	char path[TEST_DIR_MAX_SIZE] = TEST_DIR;

	struct {
		const char* desc;
		const char* path;
		void (*check)(Input*);
	} good_files[] = {
		{
			.desc = "Inf in middle file\n",
			.path = I("inf_in_middle.txt"),
		},
		{
			.desc = "Inf in end file\n",
			.path = I("inf_in_end.txt"),
			.check = check_deadline_of_first_row_is_infinity
		},
		{
			.desc = "Simple correct\n",
			.path = I("good.txt"),
		}
	};

	for (int i = 0; i < sizeof(good_files) / sizeof(good_files[0]); i++) {
		printf("File should be ok: %s", good_files[i].desc);
		if(make_file_path_relative_to_src_file(good_files[i].path, &path)) {
			exit(1);
		};
		assert_eq(read_input_from_file(path, &input), 0, "%d", "%d", "");
		if (good_files[i].check) {
			good_files[i].check(&input);
		}
	}

	struct {
		const char* desc;
		const char* path;
		const void (* check)(Input*);
	} bad_files[] = {
		{
			.desc = "Invalid path check\n",
			.path = "asdhjasdhaljsdh"
		},
		{
			.desc = "Empty file\n",
			.path = I("empty.txt"),
		},
		{
			.desc = "Longer than fit in buffer\n",
			.path = I("longer_than_buffer.txt"),
		},
		{
			.desc = "No header\n",
			.path = I("no_header.txt"),
		},
		{
			.desc = "Non integer\n",
			.path = I("non_integer.txt"),
		},
		{
			.desc = "Unknown property\n",
			.path = I("unknown_property.txt")
		}
	};
	for (int i = 0; i < sizeof(bad_files) / sizeof(bad_files[0]); i++) {
		printf("File should fail gracefully: %s", bad_files[i].desc);
		if (make_file_path_relative_to_src_file(bad_files[i].path, &path)) {
			exit(1);
		};
		assert(read_input_from_file(path, &input) != 0, "");
		if (bad_files[i].check) {
			bad_files[i].check(&input);
		}
	}
	return 0;
}
