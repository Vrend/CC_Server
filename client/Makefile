# Copyright (C) 2019 Vrend
#
# This file is part of CC_Server.
#
# CC_Server is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CC_Server is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# long with CC_Server.  If not, see <https://www.gnu.org/licenses/>.


TARGET = client

CC = g++

CFLAGS = -g -Wall

DEPS = client.h client_dependencies.h commands.h

OBJ = client.o commands.o

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^

%o: %cpp $(DEPS)
		$(CC) -c $(CFLAGS) -o $a $<

clean:
		$(RM) $(TARGET) *.o
