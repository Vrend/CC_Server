/*
Copyright (C) 2019 John Broderick

This file is part of CC_Server.

CC_Server is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CC_Server is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
long with CC_Server.  If not, see <https://www.gnu.org/licenses/>.
*/

// Dependencies go here

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include <pthread.h>
#include <string>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

//Dependencies end here

using namespace std;
