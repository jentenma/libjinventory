/*
 *  "logger.c" 
 *
 *  (c) COPYRIGHT 2020 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/dpump.h>
/* #include <sys/libpfsd.h> */
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <stdint.h>
#include "libjinventory.h"

/*
  provide callback support for logging. This API will only work
  if the caller has the ability to fill in the data structure
  Otherwise leave it alone.
*/
static void libjinventory_debug(char *format, ...);
static void libjinventory_info(char *format, ...);
static void libjinventory_warn(char *format, ...);
static void libjinventory_error(char *format, ...);
static void libjinventory_critical(char *format, ...);

static struct libjinventory_loggers lji_logger = {
	.debug = (void (*)(char *,...))libjinventory_debug,
	.info = (void (*)(char *,...))libjinventory_info,
	.warn = (void (*)(char *,...))libjinventory_warn,
	.error = (void (*)(char *,...))libjinventory_error,
	.critical = (void (*)(char *,...))libjinventory_critical
};

struct libjinventory_loggers *lji_logger_ptr =&lji_logger;

struct libjinventory_loggers *libjinventory_init_loggers(struct libjinventory_loggers *logger_ptr)
{

	/* Use local copies */
	if (logger_ptr != NULL ){
		lji_logger_ptr->debug = logger_ptr->debug;
		lji_logger_ptr->info = logger_ptr->info;
		lji_logger_ptr->warn = logger_ptr->warn;
		lji_logger_ptr->error = logger_ptr->error;
		lji_logger_ptr->critical = logger_ptr->critical;
	}
	return lji_logger_ptr;
}

#define dcp_p_red "\033[1;31m"
#define dcp_p_green "\033[1;32m"
#define dcp_p_yellow "\033[1;33m"
#define dcp_p_none "\033[0m"

static void libjinventory_debug(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_green"DEBUG: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	printf("%s",buf);
	return;
}

static void libjinventory_info(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_green"INFO: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	printf("%s",buf);
	return;
}

static void libjinventory_warn(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_yellow"WARN: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	printf("%s",buf);
	return;
}

static void libjinventory_error(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_red"ERROR: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	printf("%s",buf);
	return;
}

static void libjinventory_critical(char *format, ...)
{
	va_list ap;
	char buf[256];
	int len;
	memset(buf,0,256);
	strcpy(buf,dcp_p_red"CRITICAL: "dcp_p_none);
	len = strlen(buf);
	va_start(ap, format);
	vsnprintf(buf+len, 256-len, format, ap);
	printf("%s",buf);
	return;
}

void libjinventory_logging_test(struct libjinventory_loggers *ljip)
{
	if (ljip != NULL)
		libjinventory_init_loggers(ljip);

	lji_logger_ptr->debug("This is a debug message\n");
	lji_logger_ptr->info("This is a info message\n");
	lji_logger_ptr->warn("This is a warn message\n");
	lji_logger_ptr->error("This is a error message\n");
	lji_logger_ptr->critical("This is a critical message\n");
}
