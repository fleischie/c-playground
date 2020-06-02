/******************************************************************************
 *
 * Retrieve and output currently set keyboard layout via sway ipc
 * methods.
 *
 *****************************************************************************/
#include <errno.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ipc.h"

#define TIMEOUT_DEFAULT_SEC 5

/**
 * translate_xkb_layout_to_iso_code - Simple translation function to
 *                                    shorten the full xkb layout
 *                                    specifier to a digestable format
 */
static const char *
translate_xkb_layout_to_iso_code(const char *input)
{
  if (0 == strncasecmp(input, "english", 7)) {
    return "en";
  } else if (0 == strncasecmp(input, "russian", 7)) {
    return "ru";
  }

  return "";
}

static int
output_keyboard_layout(int socketfd, uint64_t vendor_id, uint64_t product_id)
{
  uint32_t len = 0;
  char *res = ipc_dispatch_command(socketfd, IPC_GET_INPUTS, &len);

  /* parse output json and isolate keyboard layout */
  json_object *obj = json_tokener_parse(res);
  if (!obj) {
    fprintf(stderr, "%s\n", "Unable to parse IPC response");
    return 1;
  }

  /* iterate through json object array, compare with desired
   * vendor/product and output formatted keyboard layout */
  int result = 0;
  for (size_t i = 0; i < json_object_array_length(obj); i++) {
    json_object *item = json_object_array_get_idx(obj, i);
    json_object *item_type, *item_vendor, *item_product;
    if (!json_object_object_get_ex(item, "type", &item_type) ||
        !json_object_object_get_ex(item, "vendor", &item_vendor) ||
        !json_object_object_get_ex(item, "product", &item_product)) {
      continue;
    }

    const char *type = json_object_get_string(item_type);
    uint64_t vendor = json_object_get_uint64(item_vendor);
    uint64_t product = json_object_get_uint64(item_product);

    if (vendor == vendor_id && product == product_id &&
        0 == strcmp(type, "keyboard")) {
      json_object *item_layout_name;
      if (!json_object_object_get_ex(
            item, "xkb_active_layout_name", &item_layout_name)) {
        result = 1;
        break;
      }

      const char *layout = json_object_get_string(item_layout_name);

      /* output shortened language specifier */
      printf("%s\n", translate_xkb_layout_to_iso_code(layout));
    }
  }

  free(res);

  return result;
}

int
main(int argc, char **argv)
{
  /* parse arguments
   *
   * get/default vendor/product id
   * check each argument, whether it is a specific version and whether
   * there is an option for it, default each arg to 1 on error
   *
   * the continuous flag defaults to TIMEOUT_DEFAULT_SEC seconds if it
   * is not specifically set
   */
  uint64_t vendor_id = 1;
  uint64_t product_id = 1;
  bool is_continuous = false;
  struct timespec timeout = { .tv_sec = 0, .tv_nsec = 0 };
  char *endptr;
  for (int i = 0; i < argc; i++) {
    if (0 == strcmp(argv[i], "--vendor") || 0 == strcmp(argv[i], "-V")) {
      if (i + 1 >= argc) {
        break;
      }

      endptr = "";
      errno = 0;
      vendor_id = (uint64_t) strtol(argv[i + 1], &endptr, 10);
      if ((errno == ERANGE) || (0 != errno && 0 != vendor_id) ||
          argv[i + 1] == endptr) {
        vendor_id = 1;
      }
    } else if (0 == strcmp(argv[i], "--product") ||
               0 == strcmp(argv[i], "-P")) {
      if (i + 1 >= argc) {
        break;
      }

      endptr = "";
      errno = 0;
      product_id = (uint64_t) strtol(argv[i + 1], &endptr, 10);
      if ((errno == ERANGE) || (0 != errno && 0 != product_id) ||
          argv[i + 1] == endptr) {
        product_id = 1;
      }
    } else if (0 == strcmp(argv[i], "--continuous") ||
               0 == strcmp(argv[i], "-C")) {
      /* enable continuous mode */
      is_continuous = true;

      if (i + 1 >= argc) {
        break;
      }

      endptr = "";
      errno = 0;
      int64_t timeout_sec = (uint64_t) strtol(argv[i + 1], &endptr, 10);
      if ((errno == ERANGE) || 0 != errno || 1 > timeout_sec ||
          argv[i + 1] == endptr) {
        timeout.tv_sec = TIMEOUT_DEFAULT_SEC;
      } else {
        timeout.tv_sec = timeout_sec;
      }
    }
  }

  const char *sway_socket_path = getenv("SWAYSOCK");
  if (!sway_socket_path) {
    fprintf(stderr, "%s\n", "Unable to retrieve sway socket");
    return 1;
  }
  int socketfd = ipc_open_socket(sway_socket_path);

  /* display layout continuously if requested, but at least once */
  int result;
  do {
    result = output_keyboard_layout(socketfd, vendor_id, product_id);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &timeout, NULL);
  } while (is_continuous);

  return result;
}
