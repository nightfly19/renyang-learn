if OLD_LIBS_SUPPORT

if TERMINAL_VIDEO_SUPPORT
SUBDIRS = audio-old videoin-old videoout htmlbrowser addressbook inputtone
else
SUBDIRS = audio-old htmlbrowser addressbook inputtone
endif

else

if TERMINAL_VIDEO_SUPPORT
SUBDIRS = audio videoin videoout htmlbrowser addressbook inputtone
else
SUBDIRS = audio htmlbrowser addressbook inputtone
endif

endif
