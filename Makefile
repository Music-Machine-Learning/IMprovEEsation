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
# TODO separate communication and dbs for the player

CXX_LIB_FLAGS:=-shared
CXXFLAGS:= -I include
BUILD_DIR:=build
CORE_DIR:=core

OUT_LIB_DIR:=$(BUILD_DIR)/lib

EXE_CXXFLAGS:= -I include -I $(shell pg_config --includedir) -g -L $(OUT_LIB_DIR) 
LINK_CXX_ENDFLAGS := -limpro_communication -limpro_db -lpq

all: prepare library binary

debug: CXXFLAGS+= -g
debug: EXE_CXXFLAGS+= -DDEBUG
debug: all

prepare:
	mkdir -p $(OUT_LIB_DIR)

library: libimpro_communication.so libimpro_db.so

binary: $(BUILD_DIR)/musician $(BUILD_DIR)/player $(BUILD_DIR)/director $(BUILD_DIR)/db

libimpro_communication.so : $(CORE_DIR)/communication.cpp
	$(CXX) -fPIC $(CXXFLAGS) -c $< -o $(BUILD_DIR)/libimpro_communication.o
	$(CXX) $(CXX_LIB_FLAGS) -o $(OUT_LIB_DIR)/$@ $(BUILD_DIR)/libimpro_communication.o
	rm $(BUILD_DIR)/libimpro_communication.o

libimpro_db.so : $(CORE_DIR)/db.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I $(shell pg_config --includedir) -L $(shell pg_config --libdir) -c $< -o $(BUILD_DIR)/libimpro_db.o -lpq
	$(CXX) $(CXX_LIB_FLAGS) -o $(OUT_LIB_DIR)/$@ $(BUILD_DIR)/libimpro_db.o
	rm $(BUILD_DIR)/libimpro_db.o

$(BUILD_DIR)/%: $(CORE_DIR)/%*.cpp
	$(CXX) $(EXE_CXXFLAGS) $^ core/utils.cpp core/configuration.cpp -o $@ $(LINK_CXX_ENDFLAGS)


clean:
	rm -rf $(BUILD_DIR)
