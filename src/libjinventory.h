/*
 *  "pfsd-common.h" 
 *
 *  (c) COPYRIGHT 2016 IBM Corp.
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
 *
 */

#if !defined (__JINVENTORY_COMMON_H__)
#define __JINVENTORY_COMMON_H__

/*
 * Comment out to not show virtual interfaces. Be aware that bond interfaces
 * are considered virtual.
 */
#define NET_SHOW_VIRTUAL_INTERFACES

struct jinventory_dpdr_info {
	struct dp_ioc_attr *dpi;
	int fd;
};
	
struct jinventory_info {
	unsigned long long data;
	int num_drives;
	struct jinventory_dpdr_info  *dpdri;
};

struct inv_dir_info {
	char *name;
};

struct inv_dir_list {
	struct inv_dir_info *info;
	struct inv_dir_list *next;
};


/* jinventory_inventory.c */
int jinventory_drive_info( unsigned int flags, char **json_str, void **jobj );
int jinventory_num_drives( void );
int jinventory_drives_show_json( void );
char *jinventory_drives_get_json_str( void );

int jinventory_cpu_info( unsigned int flags, char **json_str, void **jobj );
int jinventory_num_cpus( void );
int jinventory_cpus_show_json( void );
char *jinventory_cpus_get_json_str( void );

int jinventory_net_info( unsigned int flags, char **json_str, void **jobj );
int jinventory_num_nets( void );
int jinventory_nets_show_json( void );
char *jinventory_nets_get_json_str( void );

int jinventory_num_sas_phys( void );
int jinventory_sas_phys_show_json( void );
char *jinventory_sas_phys_get_json_str( void );
int jinventory_sas_phy_info( unsigned int flags, char **json_str, void **jobj );

int jinventory_num_scsi_hosts( void );
int jinventory_scsi_hosts_show_json( void );
char *jinventory_scsi_hosts_get_json_str( void );
int jinventory_scsi_host_info( unsigned int flags, char **json_str, void **jobj );
void *jinventory_scsi_hosts_get_json_object( void );

char *jinventory_inventory_get_json_str( void );
char *jinventory_netstat_get_json_str( char *iface );
char *jinventory_net_interfaces_get_json_str(void);

/* get specific properties or attributes for a device */
char *jinventory_disk_property_get( unsigned int flags, char *inproperty, char *indevname );
char *jinventory_disk_sysattr_get( unsigned int flags, char *insysattr, char *indevname );

#endif /* __JINVENTORY_COMMON_H__ */
