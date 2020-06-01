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

#include "ipc.h"

/**
 * translate_xkb_layout_to_iso_code - Simple translation function to
 *                                    shorten the full xkb layout
 *                                    specifier to a digestable format
 */
static const char *
translate_xkb_layout_to_iso_code(const char* input) {
  if (0 == strncasecmp(input, "english", 7)) {
    return "en";
  } else if (0 == strncasecmp(input, "russian", 7)) {
    return "ru";
  }

  return "";
}

int
main(int argc, char **argv)
{
  /* get/default vendor/product id
   * check each argument, whether it is a specific version and whether
   * there is an option for it, default each arg to 1 on error */
  uint64_t vendor_id = 1;
  uint64_t product_id = 1;
  char *endptr;
  for (int i = 0; i < argc; i++) {
    if (0 == strcmp(argv[i], "--vendor") || 0 == (strcmp(argv[i], "-V"))) {
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
               0 == (strcmp(argv[i], "-P"))) {
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
    }
  }

  const char *sway_socket_path = getenv("SWAYSOCK");
  if (!sway_socket_path) {
    fprintf(stderr, "%s\n", "Unable to retrieve sway socket");
    return 1;
  }
  int socketfd = ipc_open_socket(sway_socket_path);
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
