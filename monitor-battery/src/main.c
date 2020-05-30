#include <ctype.h>
#include <libnotify/notify.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "monitor-battery.h"

/* size of battery full path */
#define BATTERY_ID_MAX 10

/* TODO: clean this up */
#define BATTERY_CAPACITY_PATH_LENGTH 38
#define BATTERY_CAPACITY_PATH_TEMPLATE "/sys/class/power_supply/BAT%ld/capacity"
#define BATTERY_CAPACITY_SIZE 3
#define BATTERY_PATH_LENGTH 29
#define BATTERY_PATH_TEMPLATE "/sys/class/power_supply/BAT%ld"
#define BATTERY_STATUS_CHECK "Discharging"
#define BATTERY_STATUS_PATH_LENGTH 36
#define BATTERY_STATUS_PATH_TEMPLATE "/sys/class/power_supply/BAT%ld/status"
#define BATTERY_STATUS_SIZE 11
#define NOTIFICATION_LENGTH 18
#define NOTIFICATION_TEMPLATE "Battery low: %3ld%%"
#define SUSPEND_SYSCALL "systemctl suspend"

struct sysfs_bat
{
  struct stat sb;
  char *dir;
  char *status_path;
  char *capacity_path;
  char *status;
  char *capacity;
};

bool
sysfs_bat_fill(struct sysfs_bat *bat, size_t index)
{
  snprintf(bat->dir, BATTERY_PATH_LENGTH, BATTERY_PATH_TEMPLATE, index);

  /* skip non-existing paths */
  if (stat(bat->dir, &bat->sb) == -1) {
    return false;
  }

  snprintf(bat->status_path,
           BATTERY_STATUS_PATH_LENGTH,
           BATTERY_STATUS_PATH_TEMPLATE,
           index);
  snprintf(bat->capacity_path,
           BATTERY_CAPACITY_PATH_LENGTH,
           BATTERY_CAPACITY_PATH_TEMPLATE,
           index);

  FILE *status_file = fopen(bat->status_path, "r");
  FILE *capacity_file = fopen(bat->capacity_path, "r");

  fread(bat->status, BATTERY_STATUS_SIZE, sizeof(char), status_file);
  fread(bat->capacity, BATTERY_CAPACITY_SIZE, sizeof(char), capacity_file);

  fclose(status_file);
  fclose(capacity_file);

  return true;
}

/**
 * sysfs_bat_free - dealloc and deassign sysfs_bat structure
 */
void
sysfs_bat_free(struct sysfs_bat *bat)
{
  free(bat->capacity);
  free(bat->capacity_path);
  free(bat->dir);
  free(bat->status);
  free(bat->status_path);

  bat->capacity = 0;
  bat->capacity_path = 0;
  bat->dir = 0;
  bat->status = 0;
  bat->status_path = 0;
}

/**
 * send_battery_charge_notification - create and dispatch notification
 */
void
send_battery_charge_notification(long value)
{
  char *body = calloc(NOTIFICATION_LENGTH, sizeof(char));
  snprintf(body, NOTIFICATION_LENGTH, NOTIFICATION_TEMPLATE, value);

  NotifyNotification *notification =
    notify_notification_new(APP_NAME, body, NULL);
  notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);
  notify_notification_show(notification, NULL);

  /* cleanup */
  g_object_unref(G_OBJECT(notification));
  free(body);
}

int
main(void)
{
  struct sysfs_bat bat = {
    .capacity = calloc(BATTERY_CAPACITY_SIZE, sizeof(char)),
    .capacity_path = calloc(BATTERY_CAPACITY_PATH_LENGTH, sizeof(char)),
    .dir = calloc(BATTERY_PATH_LENGTH, sizeof(char)),
    .status = calloc(BATTERY_STATUS_SIZE, sizeof(char)),
    .status_path = calloc(BATTERY_STATUS_PATH_LENGTH, sizeof(char)),
  };

  if (!notify_init(APP_NAME)) {
    fprintf(
      stderr, "Error: %s\n", "Unable to initialize the notification system");
    return 1;
  }

  for (size_t i = 0; i < BATTERY_ID_MAX; i++) {
    sysfs_bat_fill(&bat, i);

    /* go to next iteration if battery is not discharging */
    if (0 != strcmp(bat.status, BATTERY_STATUS_CHECK)) {
      continue;
    }

    /* handle capacity value appropriately */
    long capacity = strtol(bat.capacity, NULL, 10);
    if (10 > capacity && 5 <= capacity) {
      /* warn about low battery status */
      send_battery_charge_notification(capacity);
      break;
    } else if (5 > capacity) {
      /* cleanup, suspend via sys call, and exit program */
      sysfs_bat_free(&bat);
      notify_uninit();
      system(SUSPEND_SYSCALL);
      return 0;
    }
  }

  /* cleanup */
  sysfs_bat_free(&bat);
  notify_uninit();

  return 0;
}
