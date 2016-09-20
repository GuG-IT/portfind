/*
* Copyright 2015 Thorsten Geppert (tgeppert@gug-it.de). All rights reserved.
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

#ifndef PORTFIND_H_
#define PORTFIND_H_

#define INDEX_FILE				"/usr/ports/INDEX-"
#define TOKEN					"|"
#define PKG_DB					"/var/db/pkg/local.sqlite"
#define COL_COUNT				9

#define APPLICATION_VERSION		"1.6.2"
#define APPLICATION_NAME		"portfind"
#define APPLICATION_COMPANY		"GuG-IT GbR - Olena und Thorsten Geppert"
#define APPLICATION_AUTHOR		"Thorsten Geppert (tgeppert@gug-it.de)"
#define APPLICATION_DATE		"2015-08-02"
#define FIRST_RELEASE_DATE		"2010-08-30"

typedef enum col {
	PORT_NAME_VERSION     = 0,
	PORT_PATH             = 1,
	INSTALL_PATH          = 2,
	SHORT_DESCRIPTION     = 3,
	FULL_DESCRIPTION_PATH = 4,
	MAINTAINER_MAIL       = 5,
	PORT                  = 6,
	DEPENDENCIES          = 7,
	RDEPENDECIEES         = 8
} col_t;

typedef struct entry {
	char *port_name_version;
	char *port_name;
	char *port_version;
	char *installed_version;
	char *port_path;
	char *install_path;
	char *short_description;
	char *full_description_path;
	char *maintainer_mail;
	char *port;
	char *dependencies;
	char *rdependencies;
} entry_t;

void init_entry_t(entry_t *entry);
void deinit_entry_t(entry_t *entry);
void print_full_entry_t(entry_t *entry, const short full);
void print_short_entry_t(entry_t *entry, const short full);
void copy_string(char **to, const char *from);
void split_name_version(char *name_version, char **name, char **version);
//char *get_installed_version(const char *dirname, const char *name);
char *get_installed_version(const char *dbfilename, const char *name);
char *get_release();
void help(const char *programm);
void version();
int digit_count(unsigned int i);

#endif /* PORTFIND_H_ */
