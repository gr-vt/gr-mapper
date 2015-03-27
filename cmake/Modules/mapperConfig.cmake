INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_MAPPER mapper)

FIND_PATH(
    MAPPER_INCLUDE_DIRS
    NAMES mapper/api.h
    HINTS $ENV{MAPPER_DIR}/include
        ${PC_MAPPER_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREEFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    MAPPER_LIBRARIES
    NAMES gnuradio-mapper
    HINTS $ENV{MAPPER_DIR}/lib
        ${PC_MAPPER_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAPPER DEFAULT_MSG MAPPER_LIBRARIES MAPPER_INCLUDE_DIRS)
MARK_AS_ADVANCED(MAPPER_LIBRARIES MAPPER_INCLUDE_DIRS)

