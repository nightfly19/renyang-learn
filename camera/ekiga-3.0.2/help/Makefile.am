## Process this file with automake to produce Makefile.in.

if HAVE_GNOME_DOC_UTILS
include $(top_srcdir)/gnome-doc-utils.make
endif

dist-hook: doc-dist-hook

# Keeping PACKAGE_NAME to be able to install stable and snapshot in parallel
DOC_MODULE = @PACKAGE_NAME@
DOC_ENTITIES = 
DOC_INCLUDES = 
DOC_FIGURES = figures/lumi.png        \
              figures/config_d1.png   \
              figures/config_d2.png   \
              figures/config_d3.png   \
              figures/config_d4.png   \
              figures/config_d5.png   \
              figures/config_d6.png   \
              figures/config_d7.png   \
              figures/config_d8.png   \
              figures/call_d1.png     \
              figures/accounts_d1.png    \
              figures/accounts_sip.png   \
              figures/accounts_h323.png  \
              figures/audio_codecs.png
              
DOC_LINGUAS = bg de el es fi fr oc pt_BR ru sv uk
