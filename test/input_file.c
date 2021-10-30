#include "../src/imp1.c"
#include "assert.c"
#include <string.h>

char* file_path(char* path) {
	// One might think we should replace here Linux / slashes with Windows \\ slashes, but upon testing
	// I have found it is not necessary
	char* slash;
	char* FILE = __FILE__;
	char* file;
	size_t alloc_size;
	if ((slash = strrchr(FILE, '/'))) {
		size_t offset = slash - FILE;
		alloc_size = offset + strlen(path) + 1;
		//FIXME: not freed
		file = malloc(alloc_size);
		strncpy(file, FILE, alloc_size);
		memset(file + offset, 0, alloc_size - offset);
	} else {
		exit(1);
	}
	strcat(file, path);
	return file;
}

int main() {
	Input input;
	printf("Invalid path check\n");
	assert(read_input_from_file("asljdhalksjdhaslkjdh", &input) != 0, "%s", "Invalid path should error");

	printf("Good file\n");
	assert_eq(read_input_from_file(file_path("/inputs/good.txt"), &input), 0, "%d", "%d", "%s", "Good file");

	printf("Inf in middle file\n");
	assert_eq(read_input_from_file(file_path("/inputs/inf_in_middle.txt"), &input), 0, "%d", "%d", "");

	printf("Inf in end file\n");
	assert_eq(read_input_from_file(file_path("/inputs/inf_in_end.txt"), &input), 0, "%d", "%d", "");
	assert_eq(input.buffer[0].prdw[D] + 1, 0, "%lu", "%d", "Inf is actually inf");

	printf("Empty file\n");
	assert(read_input_from_file(file_path("/inputs/empty.txt"), &input) != 0, "");

	printf("Longer than fit in buffer\n");
	assert(read_input_from_file(file_path("/inputs/longer_than_buffer.txt"), &input) != 0, "");

	printf("No header\n");
	assert(read_input_from_file(file_path("/inputs/no_header.txt"), &input) != 0, "");

	printf("Non integer\n");
	assert(read_input_from_file(file_path("/inputs/non_integer.txt"), &input) != 0, "");

	printf("Unknown property\n");
	assert(read_input_from_file(file_path("/inputs/unknown_property.txt"), &input) != 0, "");

	return 0;
}
