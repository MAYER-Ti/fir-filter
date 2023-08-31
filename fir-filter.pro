TEMPLATE = app
CONFIG += console c++17 opencl
CONFIG -= app_bundle
#QT += opencl
SOURCES += \
        global.c \
        main.c



HEADERS += \
    global.h

#LIBS += -lOpenCL



#unix|win32: LIBS += -L$$PWD/'../NVIDIA GPU Computing SDK/OpenCL/common/lib/x64/' -lOpenCL

#INCLUDEPATH += $$PWD/'../NVIDIA GPU Computing SDK/OpenCL/common/lib/x64'
#DEPENDPATH += $$PWD/'../NVIDIA GPU Computing SDK/OpenCL/common/lib/x64'

DISTFILES += \
    kernel.cl

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/lib/ -lOpenCL
#else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/lib/ -lOpenCL

#INCLUDEPATH += $$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/include
#DEPENDPATH += $$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/include
#INCLUDEPATH += /usr/include/CL
#DEPENDPATH += /usr/include/CL


#unix|win32: LIBS += -L$$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/lib/ -lOpenCL

#INCLUDEPATH += $$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/include
#DEPENDPATH += $$PWD/../OpenCL-SDK-v2023.04.17-Win-x64/include


win32: LIBS += -L$$PWD/../OpenCL/OpenCL-SDK-v2023.04.17-Win-x64/lib/ -lOpenCL

INCLUDEPATH += $$PWD/../OpenCL/OpenCL-SDK-v2023.04.17-Win-x64/include
DEPENDPATH += $$PWD/../OpenCL/OpenCL-SDK-v2023.04.17-Win-x64/include
