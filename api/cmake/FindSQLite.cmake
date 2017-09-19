# - Find include and libraries for SQLITE library
# SQLITE_INCLUDE     Directories to include to use SQLITE
# SQLITE_INCLUDE-I   Directories to include to use SQLITE (with -I)
# SQLITE_LIBRARIES   Libraries to link against to use SQLITE
# SQLITE_FOUND       SQLITE was found

IF (UNIX)
    INCLUDE (UsePkgConfig)
    PKGCONFIG (sqlite3 SQLite_include_dir SQLite_link_dir SQLite_libraries SQLite_include)
    IF (SQLite_libraries)
        SET (SQLITE_FOUND TRUE)
        SET (SQLITE_LIBRARIES ${SQLite_libraries})
    ELSE (SQLite_libraries)
        SET (SQLITE_FOUND FALSE)
    ENDIF (SQLite_libraries)
ENDIF (UNIX)
