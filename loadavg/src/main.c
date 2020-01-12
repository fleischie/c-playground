#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/sysinfo.h>

// define constant values to use inside of this script
#define LOAD_ELEMENTS 3
#define LOAD_WIDTH 25
#define THRESHOLD_WARNING 0.7f
#define THRESHOLD_ALERT 1.0f

// define load status formats
#define FORMAT_STATUS_POLYBAR_OK "%.2f"
#define FORMAT_STATUS_POLYBAR_WARNING "%%{F#e6ff00}%.2f%%{F-}"
#define FORMAT_STATUS_POLYBAR_ALERT "%%{F#bd2c40}%.2f%%{F-}"
#define FORMAT_STATUS_WAYBAR_WARNING_CLASS ",\"class\":\"warning\""
#define FORMAT_STATUS_WAYBAR_ALERT_CLASS ",\"class\":\"alert\""

// define argument parsing formats
#define PARSING_FORMAT_ARGUMENT "--output=%ms"
#define PARSING_FORMAT_WAYBAR "waybar"

typedef enum
{
  LOADAVG_FORMAT_ERROR = 0,
  LOADAVG_FORMAT_POLYBAR,
  LOADAVG_FORMAT_WAYBAR,
} loadavgStyle;

/**
 * @brief Add color tags to the load value.
 *
 * Write a color-tagged string to the out variable, given the load
 * representing value. The colors are heuristically determined based on
 * the number of available processors.
 *
 * @param load      Float value to determine color by
 * @param procCount Number of processor cores in system
 * @param out       Char-pointer-pointer to write results to
 */
void
colorLoad(double load, int procCount, char** out)
{
  // check and format given load by status template
  if (load >= (THRESHOLD_ALERT * procCount)) {
    snprintf(*out, LOAD_WIDTH, FORMAT_STATUS_POLYBAR_ALERT, load);
  } else if (load >= (THRESHOLD_WARNING * procCount)) {
    snprintf(*out, LOAD_WIDTH, FORMAT_STATUS_POLYBAR_WARNING, load);
  } else {
    snprintf(*out, LOAD_WIDTH, FORMAT_STATUS_POLYBAR_OK, load);
  }
}

/**
 * @brief Get load average and color it with the given style.
 *
 * @param loadavgStyle Formatting style the parameters specified.
 */
void
processLoadavg(loadavgStyle format)
{
  // get load average from stdlib
  double load[LOAD_ELEMENTS];
  int result = getloadavg(load, LOAD_ELEMENTS);

  // handle unobtainable load avg
  if (result < 0) {
    fprintf(stderr, "Couldn't obtain load avg.\n");
    exit(1);
  }

  // warn about missing avg elements
  if (result < LOAD_ELEMENTS) {
    fprintf(stderr, "Not all elements were returned.\n");
  }

  int procCount = get_nprocs();
  if (format == LOADAVG_FORMAT_WAYBAR) {
    // get average of load from elements and assign class depending on
    // value (waybar only allows to assign one class per script output)
    float loadAvg = (load[0] + load[1] + load[2]) / 3;
    char* classStr = (loadAvg >= (THRESHOLD_ALERT * procCount))
                       ? FORMAT_STATUS_WAYBAR_ALERT_CLASS
                       : (loadAvg >= (THRESHOLD_WARNING * procCount))
                           ? FORMAT_STATUS_WAYBAR_WARNING_CLASS
                           : "";
    printf("{\"text\":\"%.2f, %.2f, %.2f\"%s}\n",
           load[0],
           load[1],
           load[2],
           classStr);
  } else {
    // format available load elements
    // - allocate memory for the formatted string
    // - format the load element with the appropriate color tags
    // - print output to stdout
    // - free the allocated memory
    char* formattedText = (char*)calloc(LOAD_WIDTH, sizeof(char));

    colorLoad(load[0], procCount, &formattedText);
    printf("%s, ", formattedText);
    colorLoad(load[1], procCount, &formattedText);
    printf("%s, ", formattedText);
    colorLoad(load[2], procCount, &formattedText);
    printf("%s\n", formattedText);

    free(formattedText);
  }
}

int
main(int argc, char** argv)
{
  loadavgStyle format = LOADAVG_FORMAT_POLYBAR;

  // look for `-o <format>`/`--output=<format>` to overwrite format
  // fall back to `polybar` for backwards compatability
  char* formatStr;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 < argc && strcmp(argv[i + 1], PARSING_FORMAT_WAYBAR) == 0) {
        format = LOADAVG_FORMAT_WAYBAR;
        break;
      }
    } else if (sscanf(argv[i], PARSING_FORMAT_ARGUMENT, &formatStr) == 1) {
      if (formatStr != NULL && strcmp(formatStr, PARSING_FORMAT_WAYBAR) == 0) {
        format = LOADAVG_FORMAT_WAYBAR;
        break;
      }
    }
  }

  // get, format, and print loadavg
  processLoadavg(format);

  // exit successfully
  return EXIT_SUCCESS;
}
