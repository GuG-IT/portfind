/*
* Copyright 2016 Thorsten Geppert (tgeppert@gug-it.de). All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without modification, are
* permitted provided that the following conditions are met:
* 
*    1. Redistributions of source code must retain the above copyright notice, this list of
*       conditions and the following disclaimer.
* 
*    2. Redistributions in binary form must reproduce the above copyright notice, this list
*       of conditions and the following disclaimer in the documentation and/or other materials
*       provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THORSTEN GEPPERT ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
* FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Thorsten Geppert OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* The views and conclusions contained in the software and documentation are those of the
* authors and should not be interpreted as representing official policies, either expressed
* or implied, of Thorsten Geppert.
*/

#include "portfind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sqlite3.h>

int main(int argc, char **argv) {
	short option_description  = 0;
	short option_path         = 0;
	short option_maintainer   = 0;
	short option_dependencies = 0;
	short option_help         = 0;
	short option_full         = 0;
	short option_normal       = 0;
	short option_version      = 0;
	short option_summary      = 0;
	const char *search        = NULL;

	int option = 0;
	while((option = getopt(argc, argv, "adpmDrhfvn")) != -1) {
		switch(option) {
			case 'a':
				option_description  = 1;
				option_path         = 1;
				option_maintainer   = 1;
				option_dependencies = 1;
				break;
			case 'd':
				option_description  = 1;
				break;
			case 'p':
				option_path         = 1;
				break;
			case 'm':
				option_maintainer   = 1;
				break;
			case 'D':
				option_dependencies = 1;
				break;
			case 'f':
				option_full         = 1;
				break;
			case 'v':
				option_version      = 1;
				break;
			case 'n':
				option_normal       = 1;
				break;
			case 'r':
				option_summary      = 1;
				break;
			case 'h':
			case '?':
				option_help = 1;
				break;
		}
	}

	if(optind != argc - 1)
		option_help = 1;
	else
		search = argv[argc - 1];

	if(option_help && !option_version) {
		help(argv[0]);
		return 0;
	}

	if(option_version) {
		version();
		return 0;
	}

	char *release = get_release();
	if(!release) {
		fprintf(stderr, "Could not determine release\n");
		return 1;
	}

	char *filename = calloc(strlen(release) + strlen(INDEX_FILE) + 1, sizeof(char));
	sprintf(filename, "%s%s", INDEX_FILE, release);
	free(release);

	FILE *file = fopen(filename, "r");
	if(!file) {
		fprintf(stderr, "Could not open %s\n", filename);
		free(filename);
		return 1;
	}
	free(filename);

	entry_t entry;
	unsigned long matches = 0;
	unsigned long results = 0;
	unsigned long whole   = 0;
	short found           = 0;
	int col               = 0;
	char *phrase          = NULL;
	short run             = 1;
	char *line            = NULL;
	char *tmp             = line;
	long length           = 0;
	char c                = 0;
	long i                = 0;
	while(run) {
		c = fgetc(file);
		length++;
		if(c == '\n' || c == EOF) {
			fseek(file, ftell(file) - length, SEEK_SET);
			line = calloc(length, sizeof(char));
			tmp = line;
			for(i = 0; i < length - 1; i++)
				*tmp++ = fgetc(file);
			fgetc(file);

			init_entry_t(&entry);

			for(phrase = strtok(line, TOKEN), col = 0; phrase && col < COL_COUNT; phrase = strtok(NULL, TOKEN), col++) {
				switch(col) {
					case PORT_NAME_VERSION:
						split_name_version(phrase, &entry.port_name, &entry.port_version);
						copy_string(&entry.port_name_version, phrase);
						break;
					case PORT_PATH:
						copy_string(&entry.port_path, phrase);
						break;
					case INSTALL_PATH:
						copy_string(&entry.install_path, phrase);
						break;
					case SHORT_DESCRIPTION:
						copy_string(&entry.short_description, phrase);
						break;
					case FULL_DESCRIPTION_PATH:
						copy_string(&entry.full_description_path, phrase);
						break;
					case MAINTAINER_MAIL:
						copy_string(&entry.maintainer_mail, phrase);
						break;
					case PORT:
						copy_string(&entry.port, phrase);
						break;
					case DEPENDENCIES:
						copy_string(&entry.dependencies, phrase);
						break;
					case RDEPENDECIEES:
						copy_string(&entry.rdependencies, phrase);
						break;
				}
			}

			whole++;
			found = 0;
			if(entry.port_name && strcasestr(entry.port_name, search)) {
				found = 1;
				matches++;
			}
			if(option_description && entry.short_description && strcasestr(entry.short_description, search)) {
				found = 1;
				matches++;
			}
			if(option_path && entry.port_path && strcasestr(entry.port_path, search)) {
				found = 1;
				matches++;
			}
			if(option_maintainer && entry.maintainer_mail && strcasestr(entry.maintainer_mail, search)) {
				found = 1;
				matches++;
			}
			if(option_dependencies && entry.dependencies && strcasestr(entry.dependencies, search)) {
				found = 1;
				matches++;
			}
			if(option_dependencies && entry.rdependencies && strcasestr(entry.rdependencies, search)) {
				found = 1;
				matches++;
			}

			if(found) {
				results++;
				entry.installed_version = get_installed_version(PKG_DB, entry.port_name);
				if(!option_normal)
					print_short_entry_t(&entry, option_full);
				else
					print_full_entry_t(&entry, option_full);
			}

			deinit_entry_t(&entry);
			free(line);
			length = 0;

			if(c == EOF)
				run = 0;
		}
	}

	fclose(file);

	if(option_summary)
		printf("\nResults: %lu / Matches: %lu / Entries: %lu\n\n", results, matches, whole);

	return 0;
}

void init_entry_t(entry_t *entry) {
	entry->port_name_version     = NULL;
	entry->port_name             = NULL;
	entry->port_version          = NULL;
	entry->installed_version     = NULL;
	entry->port_path             = NULL;
	entry->install_path          = NULL;
	entry->short_description     = NULL;
	entry->full_description_path = NULL;
	entry->maintainer_mail       = NULL;
	entry->port                  = NULL;
	entry->dependencies          = NULL;
	entry->rdependencies         = NULL;
}

void deinit_entry_t(entry_t *entry) {
	free(entry->port_name_version);
	free(entry->port_name);
	free(entry->port_version);
	free(entry->installed_version);
	free(entry->port_path);
	free(entry->install_path);
	free(entry->short_description);
	free(entry->full_description_path);
	free(entry->maintainer_mail);
	free(entry->port);
	free(entry->dependencies);
	free(entry->rdependencies);
}

void copy_string(char **to, const char *from) {
	if(!from) {
		if(to)
			*to = NULL;
		return;
	}

	*to = strdup(from);
}

void print_full_entry_t(entry_t *entry, const short full) {
	if(!entry)
		return;
	
	size_t port_title = 0;
	if(entry->port_name_version)
		port_title = strlen(entry->port_name_version);
	if(entry->maintainer_mail)
		port_title += strlen(entry->maintainer_mail) + 3;
	
	int i = 0;
	printf("\n%s", entry->port_name_version ? entry->port_name_version : "");
	if(entry->maintainer_mail)
		printf(" (%s)\n", entry->maintainer_mail);
	for(i = 0; i < port_title; i++)
		printf("=");

	printf("\n\n%s\n\n", entry->short_description ? entry->short_description : "No description avaiable");
	printf("        Port name: %s\n", entry->port_name ? entry->port_name : "No name avaiable");
	printf("     Port version: %s\n", entry->port_version ? entry->port_version : "Unkown port version");
	printf("Installed version: %s\n", entry->installed_version ? entry->installed_version : "Not installed");
	printf("             Port: %s\n", entry->port ? entry->port : "Not avaiable");
	printf("        Port path: %s\n", entry->port_path ? entry->port_path : "No port path avaiable");
	printf("     Install path: %s\n", entry->install_path ? entry->install_path : "No install path avaiable");

	if(full) {
		printf("     Dependencies: %s\n", entry->dependencies ? entry->dependencies : "No dependencies avaiable");
		printf("   R-Dependencies: %s\n", entry->rdependencies ? entry->rdependencies : "No r-dependencies avaiable");
	}

	printf("\n");
}

void print_short_entry_t(entry_t *entry, const short full) {
	if(!entry)
		return;
	
	printf(
		"%c %s (%s",
		entry->installed_version ? '+' : '-',
		entry->port_name_version ? entry->port_name_version : "n/a",
		entry->port_path ? entry->port_path : "n/a"
	);

	if(full)
		printf(
			", Version: %s [%s, %s]",
			entry->installed_version ? entry->installed_version : "n/a",
			entry->maintainer_mail ? entry->maintainer_mail : "n/a",
			entry->install_path ? entry->install_path : "n/a"
		);
	printf(")\n");
}

void split_name_version(char *name_version, char **name, char **version) {
	if(!name_version) {
		if(name)
			*name = NULL;
		if(version)
			*version = NULL;
		return;
	}

	const size_t length = strlen(name_version);
	if(length == 0) {
		if(name)
			*name = strdup(name_version);
		if(version)
			*version = NULL;
	} else {
		int i = 0;
		for(i = length - 1; i >= 0; i--)
			if(name_version[i] == '-')
				break;
		
		if(i == 0) {
			if(name)
				*name = strdup(name_version);
			if(version)
				*version = NULL;
		} else {
			if(name)
				*name = strndup(name_version, i);

			if(version) {
				name_version += i + 1;
				*version = strdup(name_version);
			}
		}
	}
}

/*char *get_installed_version(const char *dirname, const char *name) {
	if(!dirname || !name)
		return NULL;

	char *version = NULL;
	DIR *dir = opendir(dirname);
	if(!dir)
		return version;
	
	int run           = 1;
	char *tmp_name    = NULL;
	char *tmp_version = NULL;
	struct dirent *dp = NULL;
	while((dp = readdir(dir)) != NULL && run) {
		if(dp->d_type != DT_DIR)
			continue;

		split_name_version(dp->d_name, &tmp_name, &tmp_version);
		if(strcmp(tmp_name, name) == 0) {
			version = strdup(tmp_version);
			run = 0;
		}

		free(tmp_name);
		free(tmp_version);
	}

	closedir(dir);

	return version;
}*/

char *get_installed_version(const char *dbfilename, const char *name) {
	char *version = NULL;

	if(!dbfilename || !name)
		return version;

	sqlite3 *connection = NULL;
	if(sqlite3_open(dbfilename, &connection) == 0) {
		const char *pre_query = "SELECT version FROM packages WHERE name = ";
		size_t query_length   = strlen(pre_query) + 3 + strlen(name);
		char *query           = calloc(query_length, sizeof(char));
		snprintf(query, query_length, "%s'%s'", pre_query, name);

		char **result;
		int rows     = 0;
		int columns  = 0;
		char *error  = NULL;
		int rc = sqlite3_get_table(
			connection,
			query,
			&result,
			&rows,
			&columns,
			&error
		);

		if(rc == SQLITE_OK) {
			if(rows > 0)
				version = strdup(result[1]);

			sqlite3_free_table(result);
		}

		sqlite3_close(connection);
	}

	return version;
}

char *get_release() {
	size_t length = 0;
	sysctlbyname("kern.osrelease", NULL, &length, NULL, 0);
	if(length == 0)
		return NULL;

	char *release     = calloc(length, sizeof(char));
	char *tmp_release = release;
	char *version     = calloc(length, sizeof(char));
	char *tmp_version = version;
	sysctlbyname("kern.osrelease", release, &length, NULL, 0);
	char c = *release;
	while(c != '.' && c != '\0') {
		*tmp_version++ = c;
		c = *(++tmp_release);
	}

/*	while(*tmp_release != '-' && *tmp_release != '\0')
		tmp_release++;

	if(strcmp(tmp_release, "-CURRENT") == 0) {
		int real_version = atoi(version) - 1;
		const size_t digits = digit_count(real_version);
		version = realloc(version, sizeof(char) * (digits + 1));
		sprintf(version, "%i", real_version);
	}*/

	free(release);

	return version;
}

void help(const char *program) {
	const size_t length = strlen(APPLICATION_NAME)
						+ strlen(APPLICATION_VERSION)
						+ strlen(APPLICATION_AUTHOR)
						+ strlen(APPLICATION_COMPANY)
						+ strlen(APPLICATION_DATE)
						+ 13;

	printf("\n");
	version();
	int i = 0;
	for(i = 0; i < length; i++)
		printf("=");

	printf("\n\nUsage: %s [options] search\n\n", program);
	printf("Options:\n----------\n\n");
	printf("\tWithout options the search only matches the name of the ports\n");
	printf("\t-a\t- Search in all fields\n");
	printf("\t-d\t- Search in name and in description\n");
	printf("\t-p\t- Search in name and in path\n");
	printf("\t-m\t- Search in name and maintainer\n");
	printf("\t-D\t- Search in name and dependencies\n");
	printf("\t-f\t- Show result with dependencies\n");
	printf("\t-n\t- Show normal version of results (short version is default)\n");
	printf("\t-r\t- Show result count\n");
	printf("\t-h\t- Print this help text\n");
	printf("\t-v\t- Print version number\n");
	printf("\nPlease be sure you have an actual index file or do\n\n");
	printf("\t# cd /usr/ports && sudo make index\n");
	printf("\n");
}

void version() {
	printf(
		"%s %s by %s at %s on %s (first release %s)\n",
		APPLICATION_NAME,
		APPLICATION_VERSION,
		APPLICATION_AUTHOR,
		APPLICATION_COMPANY,
		APPLICATION_DATE,
		FIRST_RELEASE_DATE
	);
}

int digit_count(unsigned int i) {
	int n = 1;

	while (i > 9) {
		n++;
		i /= 10;
	}

	return n;
}
