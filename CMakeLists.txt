cmake_minimum_required(VERSION 3.14)
project(ogma_backend)

set(CMAKE_CXX_STANDARD 14)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/libs")

include_directories(
        libs/date/include
        libs/date/include/date
        libs/sqlpp11/include
        libs/sqlpp11-connector-sqlite3/include
)

find_package(Boost 1.69 REQUIRED)
find_package(SQLite3 REQUIRED)

set(HinnantDate_ROOT_DIR "${LIB_DIR}/date/include")
set(Sqlite3_DIR "${LIB_DIR}/libsqlite3")
set(Sqlpp11_DIR "${LIB_DIR}/sqlpp11")
set(Sqlpp11Sqlite3_DIR "${LIB_DIR}/sqlpp11-connector-sqlite3")

add_subdirectory(${Sqlpp11_DIR})
add_subdirectory(${Sqlpp11Sqlite3_DIR})

file(
        GLOB Backend_SRC
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.cpp"
        "${Sqlpp11Sqlite3_DIR}/src/*.h"
        "${Sqlpp11Sqlite3_DIR}/src/*.cpp"
        "${Sqlpp11Sqlite3_DIR}/src/detail/*.h"
        "${Sqlpp11Sqlite3_DIR}/src/detail/*.cpp"
)

add_executable(ogma_backend ${Backend_SRC})
target_link_libraries(ogma_backend sqlite3 sqlpp11)
