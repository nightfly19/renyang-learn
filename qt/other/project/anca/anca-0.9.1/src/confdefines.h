#ifndef CONFDEFINES_H
#define CONFDEFINES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define MAX_DIRECTLY_VISIBLE_PLUGINS "maxDirectlyVisiblePlugins"
#define MAX_DIRECTLY_VISIBLE_PLUGINS_DEFAULT 3

#define USE_BUTTON_LABELS "useButtonLabels"
#define USE_BUTTON_LABELS_DEFAULT false

#define LABEL_POSITION_BELLOW_ICON "labelPositionBellowIcon"
#define LABEL_POSITION_BELLOW_ICON_DEFAULT true

#define REGISTER_WITH_GK "registerWithGk"
#define REGISTER_WITH_GK_DEFAULT true

#define GK_ADDR "gkAddr"
#define GK_ID "gkId"
#define GK_REGISTRATION_METHOD "gkRegistrationMethod"
#define GK_REGISTRATION_METHOD_DEFAULT EndPoint::GkAuto

#define USER_ALIASES "userAliases"

#define PORT "port"
#define PORT_DEFAULT 1720

#define SEND_VIDEO "sendVideo"
#define SEND_VIDEO_DEFAULT true
#define RECEIVE_VIDEO "receiveVideo"
#define RECEIVE_VIDEO_DEFAULT true

#define SEND_AUDIO "sendAudio"
#define SEND_AUDIO_DEFAULT true
#define RECEIVE_AUDIO "receiveAudio"
#define RECEIVE_AUDIO_DEFAULT true

#define AUTO_ANSWER "autoAnswer"
#define AUTO_ANSWER_DEFAULT false

#define FORBIDDEN_ALIASES "forbiddedAliases"

#define DO_NOT_DISTURB "doNotDisturb"
#define DO_NOT_DISTURB_DEFAULT false

#define ALWAYS_FORWARD "alwaysForward"
#define ALWAYS_FORWARD_DEFAULT false

#define BUSY_FORWARD "busyForward"
#define BUSY_FORWARD_DEFAULT false

#define NO_ANSWER_FORWARD "noAnswerForward"
#define NO_ANSWER_FORWARD_DEFAULT false

#define USE_GK_DIVERSION "useGkDiversion"
#define USE_GK_DIVERSION_DEFAULT true

#define ANSWER_TIMEOUT "answerTimeout"
#define ANSWER_TIMEOUT_DEFAULT 20

#define FORWARD_HOST "forwardHost"

#define CALL_OPTIONS_BUTTON "callOptionsButton"
#define CALL_OPTIONS_BUTTON_DEFAULT Qt::LeftButton

#define FAST_START "disableFastStart"
#define FAST_START_DEFAULT true

#define H245_TUNNELING "h245Tunneling"
#define H245_TUNNELING_DEFAULT true

#define H245_IN_SETUP "h245InSetup"
#define H245_IN_SETUP_DEFAULT true

#define RING_FILE "ringFile"
#ifdef PREL
#define RING_FILE_DEFAULT PREL "/share/anca/sounds/ringing.wav"
#else
#define RING_FILE_DEFAULT "ringing.wav"
#endif

#define RING_DELAY "ringDelay"
#define RING_DELAY_DEFAULT 1 //in seconds

#endif
