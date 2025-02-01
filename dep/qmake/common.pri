# cross compile configuration
# linux-g++-libstdc++-64
# qnx-g++-libc++
# armle-v7-qnx650-qcc-libstdc++
# x86_64-windows-msvc2019
# x86_64-linux-gnu
# aarch64le-linux-gnu
# aarch64le-qnx700-qcc-
# aarch64-unknown-nto-qnx7.0.0eabi
# armle-v7-qnx650-g++-5.2
# aarch64-linux-g++-7.1
# x86_64-linux-g++-9.0
# i686-linux-g++-5.4
# x86_64-windows-msvc2017
# x86_64-windows-mingw-7.2
# x86_64-linux-clang-7

TargetArch=unknown
TargetOs=unknown
Platform = unknown
Configuration = release
Compiler = unknown

isEmpty(QT_ARCH): message(QT_ARCH is empty. Is Qt broken?)
else: TargetArch = $$QT_ARCH

win32:TargetOs=windows
linux:TargetOs=linux
qnx:TargetOs=qnx650

Platform=$$TargetArch-$$TargetOs

*-g++*: {
    Compiler=g++
    QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden
}

*-clang*:Compiler=clang

windows {
    *-g++* {
        # MinGW
        QMAKE_CXXFLAGS += -Wno-unused-local-typedefs #-Wno-unused-parameter
        Compiler = mingw
        DEFINES+=WINVER=0x0A00 #Win10
        DEFINES+=_WIN32_WINNT=0x0A00 #Win10
    }

    *-msvc* {
        # MSVC. All others (see below).
        Compiler = msvc
    }

    *-msvc2010* {
        # MSVC 2010
        Compiler = msvc2010
    }

    *-msvc2012* {
        # MSVC 2012
        Compiler = msvc2012
    }

    *-msvc2013* {
        # MSVC 2013
        Compiler = msvc2013
        #Release with debug info for windows
        #QMAKE_CXXFLAGS_RELEASE += /Zi #C++/General/Debug Information Format = Program Database (/Zi)
        #QMAKE_LFLAGS_RELEASE += /DEBUG #C++/Linker/Generate Debug Info = Yes (/DEBUG)
    }
    
    *-msvc2015* {
        # MSVC 2015
        Compiler = msvc2015
    }
    
    *-msvc2017* {
        # MSVC 2017
        Compiler = msvc2017
    }

    *-msvc2019* {
        # MSVC 2019
        Compiler = msvc2019
    }
}

CONFIG(debug, debug|release) {
    Configuration = debug
    QMAKE_CXXFLAGS_DEBUG -= -O1
    QMAKE_CXXFLAGS_DEBUG -= -O2
    QMAKE_CXXFLAGS_DEBUG -= -O3
}

CONFIG(release, debug|release){
    CONFIG += force_debug_info separate_debug_info
    DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE *= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O3
}

QtCompatibleVersion = $$QT_MAJOR_VERSION'.'$$QT_MINOR_VERSION'.x'

qnx {
    #DEFINES += _QNX_SOURCE
    *-g++*  {
        # gcc-4.4.2 cannot recognize parameter -Wno-unused-local-typedefs
        #QMAKE_CXXFLAGS += -Wno-unused-parameter
        QMAKE_CXXFLAGS += -std=c++11
        DEFINES += __EXT_QNX __QNX__ __QNXNTO__ __unix__ __unix __ELF__ __ARM__ __arm__ __LITTLEENDIAN__ __ARMEL__ _MUDFLAPTH
    }

    *-qcc*  {
        Compiler = qcc
        QMAKE_CXXFLAGS += -Wc,-std=c++11
        QMAKE_CXXFLAGS += -Y _gpp # gcc 4.8.3 has GNU C++ lib for ARM
        QMAKE_LFLAGS += -Y _gpp
    }
    QMAKE_CXXFLAGS_DEBUG += -ggdb3 -D_GLIBCXX_DEBUG -Wfloat-equal -Wuninitialized -Wformat=2 -Wlogical-op
    QMAKE_CFLAGS_DEBUG   += -ggdb3 -Wjump-misses-init

    DEFINES += _GTHREAD_USE_MUTEX_INIT_FUNC
    DEFINES += _GTHREAD_USE_RECURSIVE_MUTEX_INIT_FUNC
    DEFINES += _GTHREAD_USE_COND_INIT_FUNC
    #clang* {
#        QMAKE_CXXFLAGS += -ferror-limit=20 -D__EXT_QNX -D_QNX_SOURCE -D__QNX__ -D__QNXNTO__ -D__unix__ -D__unix -D__ELF__ -D__ARM__ -D__arm__ -D__LITTLEENDIAN__ -D__ARMEL__ -D_MUDFLAPTH
    #}
    #QMAKE_CXXFLAGS_RELEASE += -g
    #QMAKE_CFLAGS_RELEASE += -g
    #QMAKE_LFLAGS_RELEASE =

#    QMAKE_CXXFLAGS_DEBUG += -finstrument-functions
#    LIBS += -lprofilingS

#    QMAKE_CXXFLAGS_DEBUG += -pg
#    QMAKE_LFLAGS_DEBUG += -pg
}

#!build_pass:message('"'$$Platform'"' platform selected', "'$$Configuration'" configuration')
!build_pass:message('"'$$TARGET build configuration: qt$$QtCompatibleVersion '/' $$Platform-$$Compiler '/' $$Configuration'"')

# Good way is to use makepec folder name.
#message(QMAKESPEC $$QMAKESPEC)
#MSPEC_NAME=$$section(QMAKESPEC, "/", -1, -1)
#message(MSPEC_NAME $$MSPEC_NAME)

PROJECT_ROOT_DIR = $$PWD/../..
GLOBAL_INCLUDE_DIR = $$PROJECT_ROOT_DIR/include
GLOBAL_SRC_DIR = $$PROJECT_ROOT_DIR/src

TARGET_PUBLIC_HEADERS_DIR = $$GLOBAL_INCLUDE_DIR/$$TARGET

INCLUDEPATH += $$GLOBAL_INCLUDE_DIR
               #$$PWD/../commondef \
               #../dependencies/include


DESTDIR      = $$PROJECT_ROOT_DIR/bin/qt$$QtCompatibleVersion/$$Platform-$$Compiler/$$Configuration/

OBJECTS_DIR  = $$PROJECT_ROOT_DIR/obj/qt$$QtCompatibleVersion/$$Platform-$$Compiler/$$Configuration/$$TARGET

MOC_DIR      = $$PROJECT_ROOT_DIR/moc/qt$$QtCompatibleVersion/$$Platform-$$Compiler/$$Configuration/$$TARGET

UI_DIR       = $$PROJECT_ROOT_DIR/ui/qt$$QtCompatibleVersion/$$Platform-$$Compiler/$$TARGET

#PUBLIC_HEADERS_FILES += commonhelper.h \
#                        propertydefines.h \
#                        sleephelper.h \
#                        typetraits.h

SRC_TO_INCLUDE_RELATIVE_PATH = ../include
INCLUDE_TO_SRC_RELATIVE_PATH = ../src

PROJECT_IN_GLOBAL_INCLUDE_DIR  = $$GLOBAL_INCLUDE_DIR/$$TARGET
#PROJECT_IN_GLOBAL_SRC_DIR = GLOBAL_INCLUDE_DIR/$$TARGET

#PUBLIC_HEADERS = include/*

#target_headers.files = $$PUBLIC_HEADERS
#target_headers.path  = $$PROJECT_IN_GLOBAL_INCLUDE_DIR

#mytarget.target = .buildfile
#mytarget.commands = touch $$mytarget.target
#QMAKE_EXTRA_TARGETS += mytarget
#POST_TARGETDEPS += mytarget

#INSTALLS += target_headers

LIBS        += -L$$DESTDIR
#LIBS += -L$$PROJECT_ROOT_DIR/3rdparty/lib/qt$$QtCompatibleVersion/$$Platform-$$Compiler/$$Configuration
#Backward compatibility for lib path
#LIBDIR_BACKWARD = $$PROJECT_ROOT_DIR/3rdparty/lib/qt$$QT_VERSION/
#exists($$LIBDIR_BACKWARD){
#    LIBS += -L$$PROJECT_ROOT_DIR/3rdparty/lib/qt$$QT_VERSION/$$Platform-$$Compiler/$$Configuration
#    message(Library search path $$PROJECT_ROOT_DIR/3rdparty/lib/qt$$QT_VERSION/$$Platform-$$Compiler/$$Configuration added for backward compatibility)
#}

#-L../dependencies/lib

#UI_DIR

# Uncomment next line and all child libraries will be built as dynamic.
#DEFINES  += DYNAMIC_LIBS_BUILD

# Uncomment next line and all child libraries will be built as static.
#DEFINES  += STATIC_LIBS_BUILD

#win32 {
# 1 - headers list
# 2 - project .pro location
defineReplace(createPublicHeaderAliases) {

    PUBLIC_HEADERS = $$1
    PROJ_PWD = $$2

    #TODO add error check
    INCLUDE_DIR = $$basename(GLOBAL_INCLUDE_DIR)
    contains(QMAKE_HOST.os, Linux):{
        system(mkdir -p $${PROJECT_IN_GLOBAL_INCLUDE_DIR})
        ECHO_QUOTE=$$escape_expand(\')
    } else:{
        system(cd $${PROJECT_ROOT_DIR} & if not exist $$INCLUDE_DIR mkdir $$INCLUDE_DIR)
        system(cd $${GLOBAL_INCLUDE_DIR} & if not exist $$TARGET mkdir $$TARGET)
    }
    for(FILE, PUBLIC_HEADERS) {
        SOURCEFILE_PATH = $${FILE}
        SOURCEFILE_NAME = $$basename(SOURCEFILE_PATH)
        TARGERFILEPATH = $${PROJECT_IN_GLOBAL_INCLUDE_DIR}/$${SOURCEFILE_NAME}
        TF_TEXT = include
        TF_TEXT2 = $$join(TF_TEXT, $$LITERAL_HASH, $$LITERAL_HASH)

        ABS_PATH = $${PROJ_PWD}
        PATH_BASE = $${ABS_PATH}

        TF_TEXT4 = $$TF_TEXT2 $$escape_expand(\")$${PROJ_PWD}/$$SOURCEFILE_PATH$$escape_expand(\")
        system(echo $${ECHO_QUOTE}$${TF_TEXT4}$${ECHO_QUOTE} > $${TARGERFILEPATH})
        COPY_SRC=$${GLOBAL_INCLUDE_DIR}/$${PROJ_PWD}/$${SOURCEFILE_PATH}
        COPY_DST=$${TARGERFILEPATH}
        contains(QMAKE_HOST.os, Windows) {
            COPY_SRC ~= s,/,\\,g
            COPY_DST ~= s,/,\\,g
        }
        publish_headers.commands +=$$QMAKE_COPY $${COPY_SRC} $${COPY_DST} $$escape_expand(\\n\\t)
    }

    !build_pass:message(public headers links will be created in '"'$${PROJECT_IN_GLOBAL_INCLUDE_DIR}'"' after qmake execute)  
#    message(publish commands are: $${publish.commands})
    export (publish_headers.commands)
    !build_pass:message(public headers files will be created in '"'$${PROJECT_IN_GLOBAL_INCLUDE_DIR}'"' after make publish_headers)

    greaterThan(QT_MAJOR_VERSION, 4) {
    return
    }
}

QMAKE_EXTRA_TARGETS += publish_headers

#}

export(createPublicHeaderFiles)

#GGG = $$createPublicHeaderAliases($$PUBLIC_HEADERS_FILES,$$PWD)

#QMAKE_HOST
#.arch	Host architecture
#.os	Host OS
#.cpu_count	Number of available cpus
#.name	Host computer name
#.version	Host OS version number
#.version_string	Host OS version string
#for example:
#win32-g++:contains(QMAKE_HOST.arch, x86_64):{
#    message("Host is 64bit")
#    ...
#}


#defineReplace(PublicHeaderFiles){
#    CMD_CREATE_DIR = $$quote( (if not exist $${PROJECT_IN_GLOBAL_INCLUDE_DIR} mkdir $${PROJECT_IN_GLOBAL_INCLUDE_DIR}) )
#    QMAKE_POST_LINK += $$replace(CMD_CREATE_DIR, / , \\)
#    for(FILE, PUBLIC_HEADERS_FILES) {
#        #QMAKE_POST_LINK +=$$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}$$escape_expand(\n\t))
#        SOURCEFILE_PATH = $${FILE}
#        SOURCEFILE_NAME = $$basename(SOURCEFILE_PATH) #$$replace(SOURCEFILE_PATH, $$dirname(SOURCEFILE_PATH), )
#        TARGERFILEPATH = $${PROJECT_IN_GLOBAL_INCLUDE_DIR}/$${SOURCEFILE_NAME}
#        #!exists($$PWD/$$THISFILEPATH)
#        #{
#        #    message($$PWD/$$THISFILEPATH not exists)
#        #}
#        #message($$SOURCEFILE_PATH)
#        #message($$SOURCEFILE_NAME)
#        #message($$TARGERFILEPATH)
#        #
#        #message($$basename(SOURCEFILE_PATH))
#        #message($$basename(SOURCEFILE_NAME))
#        #message($$basename(TARGERFILEPATH))
#
#        TF_TEXT = include
#        #TF_TEXT2 = $$join(TF_TEXT, $$LITERAL_HASH, $$LITERAL_HASH) $${PWD}/$$SOURCEFILE_PATH
#        TF_TEXT2 = $$join(TF_TEXT, $$LITERAL_HASH, $$LITERAL_HASH) #$$system_quote($${PWD}/$$SOURCEFILE_PATH)
#        #TF_TEXT3 = $$join(TF_TEXT, $$LITERAL_HASH, ) $$system_quote($${PWD}/$$SOURCEFILE_PATH)
#
#        ABS_PATH = $${PWD}
#        REL_PATH = ./../$$INCLUDE_TO_SRC_RELATIVE_PATH/$$TARGET
#        PATH_BASE = $${REL_PATH} #$${ABS_PATH}
#        TF_TEXT4 = $$TF_TEXT2 $$escape_expand(\")$${PATH_BASE}/$$SOURCEFILE_PATH$$escape_expand(\")
#
#        #TF_TEXT2 = "$$TF_TEXT2"
#
#        #message($$TF_TEXT)
#        #message($$TF_TEXT2)
#        message($$REL_PATH)
#        message($$TF_TEXT4)
#
#        #QMAKE_POST_LINK +=$$quote( & ( cmd @echo off & @echo $$system_quote($${TF_TEXT2}) > $${TARGERFILEPATH} ) )
#        QMAKE_POST_LINK +=$$quote( & ( cmd @echo off & @echo $${TF_TEXT4} > $${TARGERFILEPATH} ) )
#    }
#
#    export(QMAKE_POST_LINK)
#    message($$QMAKE_POST_LINK)
#
#    #copy /b NUL $${PROJECT_IN_GLOBAL_INCLUDE_DIR}/EmptyFile.txt
#    #echo 'Hello, world.' >foo.txt
#    greaterThan(QT_MAJOR_VERSION, 4) {
#    return
#    }
#    !greaterThan(QT_MAJOR_VERSION, 4) {
#        return ()
#    }
#    #lessThan(QT_MAJOR_VERSION, 5): return (1)
#}

#defineReplace(headersAndSources) {
#    variable = $$1
#    names = $$eval($$variable)
#    headers =
#    sources =
#
#    for(name, names) {
#        header = $${name}.h
#        exists($$header) {
#            headers += $$header
#        }
#        source = $${name}.cpp
#        exists($$source) {
#            sources += $$source
#        }
#    }
#    return($$headers $$sources)
#}




