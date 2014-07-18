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
# TODO a lot of dependencies checks here!

cmake_minimum_required(VERSION 2.6)
project(IMprovEEsation)

set (IMprovEEsation_VERSION_MAJOR 0)
set (IMprovEEsation_VERSION_MINOR 1)

set (CORE_SOURCE_PROJECT_DIR "${PROJECT_SOURCE_DIR}/core")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(improveesation_communication SHARED
	"${CORE_SOURCE_PROJECT_DIR}/communication.cpp")

set(EXTRA_LIBRARIES improveesation_communication)

file(GLOB DirectorSources "${CORE_SOURCE_PROJECT_DIR}/director_*.cpp")
file(GLOB PlayerSources "${CORE_SOURCE_PROJECT_DIR}/player_*.cpp")
file(GLOB MusicianSources "${CORE_SOURCE_PROJECT_DIR}/musician_*.cpp")

add_executable(director_communication_test ${DirectorSources})
target_link_libraries(director_communication_test ${EXTRA_LIBRARIES})

add_executable(player_communication_test ${PlayerSources})
target_link_libraries(player_communication_test ${EXTRA_LIBRARIES})

add_executable(musician_communication_test ${MusicianSources})
target_link_libraries(musician_communication_test ${EXTRA_LIBRARIES})
