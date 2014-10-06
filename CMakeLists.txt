###############################################################################
#  Cmake files for the IMprovEEsation project                                 #
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

cmake_minimum_required(VERSION 2.6)
project(IMprovEEsation)

set(IMprovEEsation_VERSION_MAJOR 0)
set(IMprovEEsation_VERSION_MINOR 1)

set(CORE_SOURCE_PROJECT_DIR "${PROJECT_SOURCE_DIR}/core")
set(TOOLS_SOURCE_PROJECT_DIR "${PROJECT_SOURCE_DIR}/tools")

set(CMAKE_MODULES_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (DEFINED IMPROVEESATION_DEBUG AND IMPROVEESATION_DEBUG)
	MESSAGE(STATUS "Compiling with debug flag.")
	ADD_DEFINITIONS(-D DEBUG)
else()
	MESSAGE(STATUS "Compiling without debug flag.")
	REMOVE_DEFINITIONS(-D DEBUG)
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

include(CheckFunctionExists)

check_function_exists(readv HAVE_READV_FUNCTION)

if(NOT HAVE_READV_FUNCTION)
	    message(FATAL_ERROR "ERROR: required iovec support not found")
endif(NOT HAVE_READV_FUNCTION)

add_library(improveesation_communication SHARED
	"${CORE_SOURCE_PROJECT_DIR}/communication.cpp")

add_library(improveesation_db SHARED
	"${CORE_SOURCE_PROJECT_DIR}/db.cpp")

find_path(PostgreSQL_TYPE_INCLUDE_DIR
		NAMES catalog/pg_type.h
		PATHS
# Look in other places.
		${PostgreSQL_ROOT_DIRECTORIES}
		PATH_SUFFIXES
		postgresql
		pgsql/server
		postgresql/server
		include/server
# Help the user find it if we cannot.
	DOC "The ${PostgreSQL_INCLUDE_DIR_MESSAGE}"
)

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIR})
set (POSTGRES_DB_LIBRARY ${PostgreSQL_LIBRARIES})

set(EXTRA_NET_LIBRARIES improveesation_communication)
set(EXTRA_DB_LIBRARIES improveesation_db pq)

file(GLOB DirectorSources
     "${CORE_SOURCE_PROJECT_DIR}/director_*.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/configuration.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/utils.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/midi_writer.cpp")
file(GLOB PlayerSources
     "${CORE_SOURCE_PROJECT_DIR}/player_*.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/configuration.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/utils.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/midi_writer.cpp")
file(GLOB MusicianSources
     "${CORE_SOURCE_PROJECT_DIR}/musician_*.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/configuration.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/utils.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/midi_writer.cpp")
file(GLOB DbTestSources
     "${CORE_SOURCE_PROJECT_DIR}/db_*.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/configuration.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/utils.cpp"
     "${CORE_SOURCE_PROJECT_DIR}/midi_writer.cpp")


add_executable(director ${DirectorSources})
target_link_libraries(director ${EXTRA_NET_LIBRARIES} ${EXTRA_DB_LIBRARIES})

add_executable(player ${PlayerSources})
target_link_libraries(player ${EXTRA_NET_LIBRARIES} ${EXTRA_DB_LIBRARIES})

add_executable(musician ${MusicianSources})
target_link_libraries(musician ${EXTRA_NET_LIBRARIES} ${EXTRA_DB_LIBRARIES})

add_executable(db_test ${DbTestSources})
target_link_libraries(db_test ${EXTRA_DB_LIBRARIES} ${POSTGRES_DB_LIBRARY})

# tools
file(GLOB ResetDbSources
     "${TOOLS_SOURCE_PROJECT_DIR}/resetdb.c")

add_executable(resetdb ${ResetDbSources})
