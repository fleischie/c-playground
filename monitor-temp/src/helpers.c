#include <errno.h>
#include <stdlib.h>
#include <libnotify/notify.h>
#include <sensors/sensors.h>

#include <monitor-temp.h>

#include "helpers.h"

void
print_help (char *prog)
{
	printf(
			"Usage: %s\n\n"
			"Options:\n"
			"\t--help, -h    \tPrint this help and exits\n"
			"\t--version, -v \tPrint version of this program and\n"
			"\t              \tvarious subsystems\n"
			"\t--interval, -i\tSet interval of temperature reports\n"
			"\t              \tin minutes, default: 15\n"
			"\t--oneshot, -o \tReport temperature once\n",
			prog);
	quit(EXIT_SUCCESS);
}

void
print_version (char *prog)
{
	printf(
			"Application information:\n"
			"\tapplication  :: %s\n"
			"\tname         :: %s\n"
			"\tversion      :: %d.%d\n",
			prog,
			APP_NAME,
			MONITOR_TEMP_VERSION_MAJOR,
			MONITOR_TEMP_VERSION_MINOR);

	if (print_server_info()) {
		quit(EXIT_FAILURE);
	}

	printf(
			"Sensors information:\n"
			"\tversion      :: %s\n",
			libsensors_version);

	quit(EXIT_SUCCESS);
}

int
print_server_info (void)
{
	char *ret_name;
	char *ret_vendor;
	char *ret_version;
	char *ret_spec_version;

	ret_name = (char *) calloc(sizeof (char), NAME_MAX);
	ret_vendor = (char *) calloc(sizeof (char), NAME_MAX);
	ret_version = (char *) calloc(sizeof (char), NAME_MAX);
	ret_spec_version = (char *) calloc(sizeof (char), NAME_MAX);

	if (!notify_get_server_info(
				&ret_name,
				&ret_vendor,
				&ret_version,
				&ret_spec_version)) {
		fprintf(
				stderr,
				ERROR_TEMPLATE,
				"Unable to read server information");

		// free server info variables
		free(ret_name);
		free(ret_vendor);
		free(ret_version);
		free(ret_spec_version);

		// indicate error
		return 1;
	}

	// print retrieved server info
	printf(
			"Server information:\n"
			"\tname         :: %s\n"
			"\tvendor       :: %s\n"
			"\tversion      :: %s\n"
			"\tspec version :: %s\n",
			ret_name,
			ret_vendor,
			ret_version,
			ret_spec_version);

	// free server info variables
	free(ret_name);
	free(ret_vendor);
	free(ret_version);
	free(ret_spec_version);

	// indicate success
	return 0;
}

char *
format_temperature (double temperature)
{
	char *out = (char *) calloc(sizeof (char), BODY_SIZE);
	snprintf(out, BODY_SIZE, "Temp: %.1f°C", temperature);
	return out;
}

double
get_maximum_temperature (void)
{
	// enumerate through existing chips and check temperature of
	// chips that support it, storing the highest reported one
	double max_temp = 0.0f;
	int c_nr = 0;
	while (1)
	{
		const sensors_chip_name *name = sensors_get_detected_chips(
				NULL,
				&c_nr);
		(void) c_nr;
		if (name == NULL)
		{
			break;
		}

		int f_nr = 0;
		while (1)
		{
			// get feature/subfeature list for detected chip
			const sensors_feature *feature = sensors_get_features(
					name,
					&f_nr);
			(void) f_nr;
			if (feature == NULL)
			{
				break;
			}

			// subfeature TEMP_INPUT should be available to
			// have the sensor report a chip temperature
			const sensors_subfeature *
			subfeature = sensors_get_subfeature(
					name,
					feature,
					SENSORS_SUBFEATURE_TEMP_INPUT);
			if (subfeature == NULL)
			{
				break;
			}

			// update max temperature if necessary
			double temp;
			sensors_get_value(name, subfeature->number, &temp);
			if (temp > max_temp)
			{
				max_temp = temp;
			}
		}
	}

	return max_temp;
}

void
send_notification (const char *summary, double temperature)
{
	// determine urgency
	// LOW: 0 - 50
	// NORMAL: 50 - 75
	// CRITICAL: 75 - 100
	int urgency = temperature < TEMP_LOW ?
		NOTIFY_URGENCY_LOW :
		temperature < TEMP_MID ?
			NOTIFY_URGENCY_NORMAL :
			NOTIFY_URGENCY_CRITICAL;
	// format temperature into string
	char *body = format_temperature(temperature);

	// create new notification item
	NotifyNotification *notification = notify_notification_new(
			summary,
			body,
			NULL);

	// setup and send notification
	notify_notification_set_urgency(notification, urgency);
	notify_notification_show(notification, NULL);

	// cleanup notification item
	free(body);
	g_object_unref(G_OBJECT(notification));
}

void
quit (int status)
{
	// cleanup various systems
	notify_uninit();
	sensors_cleanup();
	exit(status);
}

void
init (void)
{
	// initialize the notification system
	if (!notify_init(APP_NAME))
	{
		fprintf(
				stderr,
				ERROR_TEMPLATE,
				"Unable to initialize the notification system");
		quit(EXIT_FAILURE);
	}

	// initialize the sensor system
	if (sensors_init(NULL))
	{
		fprintf(
				stderr,
				ERROR_TEMPLATE,
				"Unable to initialize the sensor system");
		quit(EXIT_FAILURE);
	}
}

void
set_interval (long *interval, char *str)
{
	long val;
	errno = 0;
	val = strtol(optarg, NULL, 10);
	if (
			(errno == ERANGE &&
			(val == LONG_MAX || val == LONG_MIN)) ||
			val == 0)
	{
		fprintf(
				stderr,
				"Invalid interval, falling back to default: %d"
				"\n",
				DEFAULT_TIMEOUT);
		return;
	}

	*interval = val;
}
