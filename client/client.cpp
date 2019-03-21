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

#include "client.h"

bool running = true;

int main() {

	signal(SIGINT, signal_handler);
	int fd;

	struct sockaddr_in addr;

	char buffer[2048];

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd < 0) {
		cout << "Error opening socket" << endl;
		return 1;
	}

	memset(&addr, '0', sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, remote_ip, &addr.sin_addr) <= 0) {
		cout << "Bad address" << endl;
		close(fd);
		return 1;
	}

	int cres = connect(fd, (struct sockaddr*) &addr, sizeof(addr));

	if(cres < 0) {
		cout << "Connection failed...Terminating" << endl;
		close(fd);
		return 1;
	}

	while(running) {
		;
	}

	close(fd);
	cout << "closing client..." << endl;
	return 0;
}

void signal_handler(int sig) {
	running = false;
}
