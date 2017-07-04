static void print_help(void);
static __attribute__((noreturn)) void help_err(const char *const msg)
{
    fputs(msg, stderr);
    print_help();
    exit(-1);
}
