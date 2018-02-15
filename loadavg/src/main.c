#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/sysinfo.h>

// define status enum
enum {
	STATUS_OK = 0,
	STATUS_WARNING,
	STATUS_ALERT
};

// define constant values to use inside of this script
const int LOAD_ELEMENTS = 3;
const int LOAD_WIDTH = 25;
const float THRESHOLD_WARNING = 0.7f;
const float THRESHOLD_ALERT = 1.0f;
const char *LOAD_STATUS[] = {
	"%.2f",
	"%{F#e6ff00}%.2f%{F-}",
	"%{F#bd2c40}%.2f%{F-}"
};

/**
 * @brief Add color tags to the load value.
 *
 * Write a color-tagged string to the out variable, given the load
 * representing value. The colors are heuristically determined based on
 * the number of available processors.
 *
 * @param load Float value to determine color by
 * @param out  Char-pointer-pointer to write results to
 */
void
colorLoad (double load, char **out)
{
	int procCount;

	// get number of available processors
	procCount = get_nprocs();

	// check and format given load by status template
	if (load >= (THRESHOLD_ALERT * procCount))
	{
		snprintf(*out, LOAD_WIDTH, LOAD_STATUS[STATUS_ALERT], load);
	}
	else if (load >= (THRESHOLD_WARNING * procCount))
	{
		snprintf(*out, LOAD_WIDTH, LOAD_STATUS[STATUS_WARNING], load);
	}
	else
	{
		snprintf(*out, LOAD_WIDTH, LOAD_STATUS[STATUS_OK], load);
	}
}

int
main ()
{
	// define local variables
	int loadElem;
	int result;
	char **loadStr;
	double load[LOAD_ELEMENTS];

	// get load average from stdlib
	result = getloadavg(load, LOAD_ELEMENTS);

	// handle unobtainable load avg
	if (result < 0)
	{
		fprintf(stderr, "Couldn't obtain load avg.\n");
		return EXIT_FAILURE;
	}

	// warn about missing avg elements
	if (result < LOAD_ELEMENTS)
	{
		fprintf(stderr, "Not all elements were returned.\n");
	}

	// initialize load strings
	loadStr = (char **) malloc(LOAD_ELEMENTS * sizeof (char *));
	for (loadElem = 0; loadElem < LOAD_ELEMENTS; loadElem++)
	{
		loadStr[loadElem] =
			(char *) malloc(LOAD_WIDTH * sizeof (char));
	}

	// format available load elements
	// - allocate memory for the formatted string
	// - format the load element with the appropriate color tags
	// - copy formatted load text into non-temporary store
	// - free the allocated memory
	char *formattedText = (char *) malloc(LOAD_WIDTH * sizeof (char));
	for (loadElem = 0; loadElem < result; loadElem++)
	{
		colorLoad(load[loadElem], &formattedText);
		snprintf(
				loadStr[loadElem],
				LOAD_WIDTH * sizeof (char),
				"%s",
				formattedText
			);
	}
	free(formattedText);

	// print available load elements
	printf("%s, %s, %s\n", loadStr[0], loadStr[1], loadStr[2]);

	// uninitialize load strings
	for (loadElem = 0; loadElem < LOAD_ELEMENTS; loadElem++)
	{
		free(loadStr[loadElem]);
	}
	free(loadStr);

	// exit successfully
	return EXIT_SUCCESS;
}