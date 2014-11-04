cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_INSTALL_PREFIX "/")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")

project(lenovo_yoga_fan_control)

set (PROJECT_VERSION "1.0")
set (PACKAGE "${PROJECT_NAME}")
set (VERSION "${PROJECT_VERSION}")
set (PACKAGE_BUGREPORT "ilya.blan4@gmail.com")
set (PACKAGE_NAME "${PACKAGE}")
set (PACKAGE_TARNAME "${PACKAGE}")
set (PACKAGE_VERSION "${VERSION}")
set (PACKAGE_STRING "${PACKAGE} ${VERSION}")

set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(yoga_fan ${SOURCE_FILES})
install(TARGETS yoga_fan DESTINATION usr/bin)
install(
  FILES yoga_fan_daemon DESTINATION etc/init.d
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ)