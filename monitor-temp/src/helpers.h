#define BODY_SIZE 15
#define ERROR_TEMPLATE "Error: %s\n"
#define TEMP_LOW 50
#define TEMP_MID 75

void   print_help              (char *);
void   print_version           (char *);
int    print_server_info       (void);
char * format_temperature      (double);
double get_maximum_temperature (void);
void   send_notification       (const char *, double);
void   set_interval            (long *, char *);
void   init                    (void);
void   quit                    (int);
