#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void);
static __attribute__((noreturn)) void help_err(const char *const msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    print_help();
    exit(-1);
}
#endif
