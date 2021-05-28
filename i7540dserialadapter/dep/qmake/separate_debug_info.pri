!staticlib:!static:!contains(TEMPLATE, subdirs):!isEmpty(QMAKE_OBJCOPY) {
    qnx {
        debug_info_suffix = sym
        debug_info_keep = --keep-file-symbols
        debug_info_strip = --strip-debug -R.ident
    } else {
        debug_info_suffix = debug
        debug_info_keep = --only-keep-debug
        debug_info_strip = --strip-debug
    }
    equals(TEMPLATE, app) {
        TARGET_BASENAME=$${TARGET}
    } else {
        TEMP_VERSION = $$section(VERSION, ., 0, 0)
        isEmpty(TEMP_VERSION):TEMP_VERSION = 1
        TARGET_BASENAME=lib$${TARGET}.so.$${TEMP_VERSION}
        unset(TEMP_VERSION)
    }
    QMAKE_SEPARATE_DEBUG_INFO = cd \"$(DESTDIR)\" && $$QMAKE_OBJCOPY $$debug_info_keep \"$${TARGET_BASENAME}\" \"$${TARGET_BASENAME}.$${debug_info_suffix}\" && $$QMAKE_OBJCOPY $$debug_info_strip \"$${TARGET_BASENAME}\" && $$QMAKE_OBJCOPY --add-gnu-debuglink=\"$${TARGET_BASENAME}.$$debug_info_suffix\" \"$${TARGET_BASENAME}\"

    CONFIG(release, debug|release) {
        #DEBUG_DESTDIR = $$replace(DESTDIR, release, debug)
        #MOVE_DEBUG_INFO = $$QMAKE_MOVE \"$(DESTDIR)$${TARGET_BASENAME}.$${debug_info_suffix}\" \"$${DEBUG_DESTDIR}$${TARGET_BASENAME}.$${debug_info_suffix}\"
        #!isEmpty(QMAKE_POST_LINK):QMAKE_POST_LINK = $$escape_expand(\\n\\t)$$QMAKE_POST_LINK
        #QMAKE_POST_LINK = $$MOVE_DEBUG_INFO $$QMAKE_POST_LINK
        !isEmpty(QMAKE_POST_LINK):QMAKE_POST_LINK = $$escape_expand(\\n\\t)$$QMAKE_POST_LINK
        QMAKE_POST_LINK = $$QMAKE_SEPARATE_DEBUG_INFO $$QMAKE_POST_LINK
        silent:QMAKE_POST_LINK = @echo creating $@.$$debug_info_suffix && $$QMAKE_POST_LINK
    }

    separate_debug_info.target = $${TARGET_BASENAME}.$${debug_info_suffix}
    separate_debug_info.commands = $$QMAKE_SEPARATE_DEBUG_INFO
    QMAKE_EXTRA_TARGETS += separate_debug_info

    unset(TARGET_BASENAME)
}
