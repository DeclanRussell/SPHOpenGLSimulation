#-------------------------------------------------
#
# Project created by QtCreator 2012-01-26T14:46:22
#
#-------------------------------------------------

QT       += core
QT       -= gui

OBJECTS_DIR = obj
TARGET = bin/test-project-qt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS += -L ../lib64 -lsdf-lite-linuxgcc-mt

# That was for me at first
#INCLUDEPATH+=$HOME/sdflib-msc/include
INCLUDEPATH+=../include

#INCLUDEPATH+=C:\boost_1_44_0\

SOURCES += ../src/main.cpp \
    ../src/files.cpp \
    ../src/command_line_args.cpp

HEADERS += \
    ../include/sdf/signed_distance_field_from_mesh.hpp \
    ../include/files.hpp \
    ../include/command_line_args.hpp

#LIBS += -L C:\boost_1_44_0\stage\lib
LIBS += -lboost_thread
LIBS += -lboost_filesystem
LIBS += -lboost_program_options
