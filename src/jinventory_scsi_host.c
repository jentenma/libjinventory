/*
 *  "jinventory_scsi_host.c" 
 *
 *  (c) COPYRIGHT 2016 IBM Corp.
 *
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

/* need this for the cpu affinity stuff */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libudev.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include "libjinventory.h"
#include <json-c/json.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/ethtool.h>

static int jinventory_hba_info( unsigned int flags, char **json_str, void **jobj, char *hosttype );
static int verbose_debug = 0;

#define PFSD_INV_FLAG_VERBOSE  (1<<0)

int jinventory_num_scsi_hosts( void )
{
	char *json_str = NULL;
	int num_scsi_hosts = 0;
	num_scsi_hosts = jinventory_hba_info(0,&json_str,(void **)NULL,"scsi_host"); 
	if (json_str)
		free(json_str);
	return num_scsi_hosts;
}

int jinventory_scsi_hosts_show_json( void )
{
	char *json_str = NULL;
	int num_scsi_hosts = 0;
	num_scsi_hosts = jinventory_hba_info(PFSD_INV_FLAG_VERBOSE, (char **)NULL, (void **)NULL,"scsi_host"); 
	/*	num_scsi_hosts = jinventory_hba_info(PFSD_INV_FLAG_VERBOSE, &json_str, (void **)NULL);  */

	if (json_str)
		free(json_str);
	return num_scsi_hosts;
}

char *jinventory_scsi_hosts_get_json_str( void )
{
	char *json_str = NULL;
	jinventory_hba_info(0, &json_str, (void **)NULL,"scsi_host");
	return json_str;
}

void *jinventory_scsi_hosts_get_json_object( void )
{
	char *json_str = NULL;
	json_object *jobj;
	jinventory_hba_info(0, &json_str, (void **)&jobj,"scsi_host");
	return (void *)jobj;
}

/*******************************************************************
 FC Hosts
******************************************************************/

int jinventory_num_fc_hosts( void )
{
	char *json_str = NULL;
	int num_fc_hosts = 0;
	num_fc_hosts = jinventory_hba_info(0,&json_str,(void **)NULL,"fc_host"); 
	if (json_str)
		free(json_str);
	return num_fc_hosts;
}

int jinventory_fc_hosts_show_json( void )
{
	char *json_str = NULL;
	int num_scsi_hosts = 0;
	num_scsi_hosts = jinventory_hba_info(PFSD_INV_FLAG_VERBOSE, (char **)NULL, (void **)NULL,"fc_host"); 
	/*	num_scsi_hosts = jinventory_hba_info(PFSD_INV_FLAG_VERBOSE, &json_str, (void **)NULL);  */

	if (json_str)
		free(json_str);
	return num_scsi_hosts;
}

char *jinventory_fc_hosts_get_json_str( void )
{
	char *json_str = NULL;
	jinventory_hba_info(0, &json_str, (void **)NULL,"fc_host");
	return json_str;
}

void *jinventory_fc_hosts_get_json_object( void )
{
	char *json_str = NULL;
	json_object *jobj;
	jinventory_hba_info(0, &json_str, (void **)&jobj,"fc_host");
	return (void *)jobj;
}


/*
 * **********************************************************************
 *
 * Function:  int jinventory_hba_info()
 *
 * Description:
 *
 * inputs:
 * 	 unsigned int flags
 * 	 char **json_str
 *       hosttype  scsi_host or fc_host
 *
 * returns:
 *
 * side effects:
 *
 * *********************************************************************
 */
static int jinventory_hba_info( unsigned int flags, char **json_str, void **jobj, char *hosttype )
{
	int num_hba_hosts;
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry, *list_entry;
	struct udev_device *dev;
	json_object *hba_host_object;
	json_object *info_object;
	json_object *hba_host_list_object;
	char dstr[64];
	int verbose = flags & PFSD_INV_FLAG_VERBOSE;
	const char *jstr = NULL;

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		return(0);
	}
	
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, hosttype);
	/* filter out the USB devices */
	udev_enumerate_add_nomatch_sysattr(enumerate, "proc_name","usb-storage");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	num_hba_hosts = 0;

	hba_host_list_object = json_object_new_object();

	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		const char *property;
		const char *value;
		char  cfpath[256];

		memset((void *)cfpath,0,256);

		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		if ( verbose_debug )
			printf("path = %s\n", path);
		dev = udev_device_new_from_syspath(udev, path);

		/* new info object */
		info_object = json_object_new_object();

		 
		udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(dev)) {
			property = udev_list_entry_get_name(list_entry);
			value =    udev_list_entry_get_value(list_entry);
			if ( verbose_debug )
				printf("property: %s=%s\n", property, value); 
			if ( value != NULL )
				json_object_object_add(info_object, property, json_object_new_string(value));
		}

		udev_list_entry_foreach(list_entry, udev_device_get_sysattr_list_entry(dev)) {
			property = udev_list_entry_get_name(list_entry);
			value = udev_device_get_sysattr_value(dev, udev_list_entry_get_name(list_entry));
			if ( verbose_debug )
				printf("sysattr:  %s=%s\n", property, value);
			if ( value != NULL )
				json_object_object_add(info_object, property, json_object_new_string(value));
		}


		hba_host_object = json_object_new_object();
		json_object_object_add(hba_host_object,"info", info_object);

		sprintf(dstr,"%s%d", hosttype, num_hba_hosts);

		if ( verbose_debug )
			printf(" --  Adding %s\n", dstr);

		json_object_object_add(hba_host_list_object, dstr, hba_host_object);

		udev_device_unref(dev);

		num_hba_hosts++;

	}

	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	if ( verbose ){
		if ( verbose_debug )
			printf (" ** Number of %s found = %d\n", hosttype, num_hba_hosts);
		printf("%s\n", json_object_to_json_string(hba_host_list_object));
	}

	if (json_str != NULL) {
		jstr = json_object_to_json_string(hba_host_list_object);
		*json_str = malloc(strlen(jstr)+32);
		strcpy(*json_str, jstr);
	}

	if ( jobj != NULL )
		*jobj = hba_host_list_object;
	else
		json_object_put(hba_host_list_object);

	return num_hba_hosts;

}
