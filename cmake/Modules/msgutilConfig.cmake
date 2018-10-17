INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_MSGUTIL msgutil)

FIND_PATH(
    MSGUTIL_INCLUDE_DIRS
    NAMES msgutil/api.h
    HINTS $ENV{MSGUTIL_DIR}/include
        ${PC_MSGUTIL_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    MSGUTIL_LIBRARIES
    NAMES gnuradio-msgutil
    HINTS $ENV{MSGUTIL_DIR}/lib
        ${PC_MSGUTIL_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MSGUTIL DEFAULT_MSG MSGUTIL_LIBRARIES MSGUTIL_INCLUDE_DIRS)
MARK_AS_ADVANCED(MSGUTIL_LIBRARIES MSGUTIL_INCLUDE_DIRS)

