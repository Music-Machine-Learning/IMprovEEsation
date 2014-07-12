###############################################################################
# Makefile for the IMprovEEsation project                                     #
#                                                                             #
#  Copyright (C) 2014                                                         #
#  Federico Montori, Marco Melletti, Davide Berardi, Matteo Martelli          #
#                                                                             #
#  This program is free software; you can redistribute it and/or              #
#  modify it under the terms of the GNU General Public License                #
#  as published by the Free Software Foundation; either version 2             #
#  of the License, or any later version.                                      #
#                                                                             #
#  This program is distributed in the hope that it will be useful,            #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of             #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              #
#  GNU General Public License for more details.                               #
#                                                                             #
#  You should have received a copy of the GNU General Public License          #
#  along with this program; if not, write to the Free Software                #
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, #
#  USA.                                                                       #
###############################################################################
CXX_LIB_FLAGS:=-shared
CXXFLAGS:= -I include -g
BUILD_DIR:=build
CORE_DIR:=core

OUT_LIB_DIR:=$(BUILD_DIR)/lib

EXE_CXXFLAGS:= -I include -g -L $(OUT_LIB_DIR) -limpro_communication

all: prepare library binary

prepare:
	mkdir -p $(OUT_LIB_DIR)

library: libimpro_communication.so

binary: $(BUILD_DIR)/musician $(BUILD_DIR)/player $(BUILD_DIR)/director

libimpro_communication.so : $(CORE_DIR)/communication.cpp
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o $(BUILD_DIR)/libimpro_communication.o
	$(CXX) $(CXX_LIB_FLAGS) -o $(OUT_LIB_DIR)/$@ $(BUILD_DIR)/libimpro_communication.o
	rm $(BUILD_DIR)/libimpro_communication.o


$(BUILD_DIR)/%: $(CORE_DIR)/%*.cpp
	$(CXX) $(EXE_CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
