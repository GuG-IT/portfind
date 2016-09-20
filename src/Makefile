# Copyright 2016 Thorsten Geppert (tgeppert@gug-it.de). All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THORSTEN GEPPERT ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Thorsten Geppert OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of Thorsten Geppert.

CC        = cc
LINK      = cc

OBJ       = portfind.o
LIBS      = -lsqlite3 -L/usr/local/lib
INCS      = -I/usr/local/include
BIN       = portfind
CFLAGS    = -O3 -Wall
VERSION   = "1.6.3"

all: $(BIN) doc

clean:
	rm -f portfind.1.gz portfind.o portfind portfind.1

doc:
	pod2man --section=1 --center="Search the ports collection" --release=$(VERSION) ../doc/$(BIN).pod > $(BIN).1

install: all
	cp -f portfind /usr/local/bin/portfind
	gzip -f portfind.1
	cp -f portfind.1.gz /usr/local/man/man1/portfind.1.gz

$(BIN): $(OBJ)
	$(LINK) $(OBJ) -o $(BIN) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $*.c -o $*.o
