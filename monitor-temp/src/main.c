#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <monitor-temp.h>

#include "helpers.h"

int
main (int argc, char **argv)
{
	int oneshot = 0;
	long interval = DEFAULT_TIMEOUT;

	int choice;
	static struct option long_options[] =
	{
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"oneshot", no_argument, 0, 'o'},
		{"interval", required_argument, 0, 'i'},
		{0,0,0,0}
	};

	while (1)
	{
		int option_index = 0;
		choice = getopt_long(
				argc,
				argv,
				"vhoi:",
				long_options,
				&option_index);

		if (choice == -1)
			break;

		switch(choice)
		{
			case 'v':
				print_version(argv[0]);
				break;

			case 'h':
				print_help(argv[0]);
				break;

			case 'o':
				oneshot = 1;
				break;

			case 'i':
				set_interval(&interval, optarg);
				break;

			case '?':
				break;

			default:
				return EXIT_FAILURE;
		}
	}

	init();

	if (oneshot)
	{
		// report maximum temperature
		double max_temp = get_maximum_temperature();
		send_notification(APP_NAME, max_temp);
		quit(EXIT_SUCCESS);
	}

	struct timespec timeout = {
		.tv_sec = interval * 60
	};
	while (1)
	{
		// report maximum temperature
		double max_temp = get_maximum_temperature();
		send_notification(APP_NAME, max_temp);

		// delay next iteration
		clock_nanosleep(CLOCK_MONOTONIC, 0, &timeout, NULL);
	}

	quit(EXIT_SUCCESS);
}
