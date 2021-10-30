#ifdef _WIN32
#define assert(bool, fmt, ...) {\
	if(bool == 0) {\
		fprintf(stderr, "%s(%s:%d) Assertion failed: " fmt, __func__, __FILE__, __LINE__, __VA_ARGS__);\
		exit(1);\
	}\
}

#define assert_eq(val1, val2, val1format, val2format, fmt, ...) {\
	if(val1 != val2) {\
		fprintf(stderr, "L != R (" val1format " !=" val2format "): %s(%s:%d) " fmt, val1, val2, __func__, __FILE__, __LINE__, __VA_ARGS__);\
		exit(1);\
	}\
}
#else
#define assert(bool, fmt, args...) {\
	if(bool == 0) {\
		fprintf(stderr, "%s(%s:%d) Assertion failed: " fmt, __func__, __FILE__, __LINE__, ##args);\
		exit(1);\
	}\
}

#define assert_eq(val1, val2, val1format, val2format, fmt, args...) {\
	if(val1 != val2) {\
		fprintf(stderr, "L != R (" val1format " !=" val2format "): %s(%s:%d) " fmt, val1, val2, __func__, __FILE__, __LINE__, ##args);\
		exit(1);\
	}                                                                   \
}
	#endif
