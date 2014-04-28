cmake_minimum_required(VERSION 2.6)
project(testy jezykowe)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(Testy jezykowe_SRCS Testy jezykowe.cpp main.cpp)

qt4_automoc(${Testy jezykowe_SRCS})
add_executable(testy jezykowe ${Testy jezykowe_SRCS})
target_link_libraries(testy jezykowe ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS testy jezykowe RUNTIME DESTINATION bin)
