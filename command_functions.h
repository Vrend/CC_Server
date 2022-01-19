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

#include "constants.h"

// Functions begin

void command_help(string* args);

void command_run(string* args, int fd);

void command_list(string* args);

void command_upload(string* args, int fd);

void command_compile(string* args);

void command_exit(int fd);

// Functions ends
