QT -= core gui

# Nazwa tymczasowa:
TARGET = my-little-plant

# Build do biblioteki .so
TEMPLATE = lib

# Bez QT
CONFIG -= qt

SOURCES += \
    sample.cpp

HEADERS += \
    plant.h

