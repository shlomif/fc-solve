#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void);
#endif
static inline __attribute__((noreturn)) void help_err(const char *const msg)
{
#ifndef FCS_WITHOUT_CMD_LINE_HELP
    fputs(msg, stderr);
    print_help();
#endif
    exit(-1);
}
