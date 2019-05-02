#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sodium.h>

#define LOWER_CHARS "abcdefghijklmnopqrstuvwxyz"
#define UPPER_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMBERS     "0123456789"
#define SYMBOLS     "\\\"',<.>/?;:|[]{}~!@#$%^&*()_+`-="

/**
 * randomsort - Sorting function that returns random order of given
 *              items.
 *
 * @param [in] p1 First ignored parameter.
 * @param [in] p2 Second ignored parameter.
 * @return Random integer value.
 */
static int
randomsort (const void *p1, const void *p2)
{
	int r = randombytes_random();
	return r;
}

/**
 * choice_char - Return random character from given char ptr.
 *
 * @param [in] str Character pointer to return random char from.
 * @return Randomly selected char from given input.
 */
char
choice_char (const char *str)
{
	uint32_t r = randombytes_random();
	size_t l = strlen(str);

	return str[r % l];
}

/**
 * generate_passwd - Generate password from given list of string arrays.
 *
 * @param [in] arr      Array of character ptrs to generate password
 *                      with.
 * @param [in] nsources Number of elements in array.
 * @param [in] nelem    Number of elements per element in array.
 * @return Character pointer consisting 
 */
char *
generate_password (char **arr, int nsources, int nelem)
{
	char *out = calloc(sizeof (char), nsources * nelem);

	for (int i = 0; i < nsources; i++) {
		for (int j = 0; j < nelem; j++) {
			out[(i * 3) + j] = choice_char(arr[i]);
		}
	}

	return out;
}

int
main (void)
{
	if (sodium_init() < 0)
	{
		fprintf(
				stderr,
				"%s\n",
				"Sodium failed to initialize. Exitting.");
		return EXIT_FAILURE;
	}

	char *input[4] = {
		LOWER_CHARS,
		UPPER_CHARS,
		NUMBERS,
		SYMBOLS
	};
	qsort(&input[0], 4, sizeof (char *), randomsort);

	char *passwd = generate_password(input, 4, 3);
	printf("Generated password: %s\n", passwd);

	free(passwd);

	return EXIT_SUCCESS;
}
