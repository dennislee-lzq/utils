 #include <syslog.h>

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void closelog(void);

int main()
{

	openlog("test-module", LOG_CONS|LOG_PERROR, LOG_USER);

	syslog(LOG_EMERG, "%s", "this is a emerg log");
	syslog(LOG_ERR, "%s", "this is a error log");
	syslog(LOG_INFO, "%s", "this is a info log");
	syslog(LOG_DEBUG, "%s", "this is a debug log");

	syslog(LOG_EMERG, "%s", "this is a emerg log");
	return 0;
}
