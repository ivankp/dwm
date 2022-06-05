/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int focusonwheel       = 0;
static const char *fonts[]          = {
  "DejaVuSansMono Nerd Font:pixelsize=14:antialias=true:autohint=true:Book"
};
static const char dmenufont[]       =
  "DejaVuSansMono Nerd Font:pixelsize=14:antialias=true:autohint=true:Book";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
  /*               fg         bg         border   */
  [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
  [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
  /* xprop(1):
   *  WM_CLASS(STRING) = instance, class
   *  WM_NAME(STRING) = title
   */
  // class       , instance , title , tags mask , isfloating , monitor
  // { "Gimp"    , NULL     , NULL  , 0         , 1          , -1 }    ,
  // { "firefox" , NULL     , NULL  , 1 << 1    , 0          , -1 }    ,
  // { "zoom"       , NULL     , NULL  , 1 << 4    , 0          , -1 }    ,
  { NULL , NULL , "Zoom Meeting" , 1 << 4 , 0 , -1 } ,
  { NULL , NULL , "Zoom -"       , 1 << 8 , 0 , -1 } ,
  { NULL , NULL , NULL           , 0      , 0 , -1 } ,
};

/* layout(s) */
static const float mfact     = 0.5; // factor of master area size [0.05..0.95]
static const int nmaster     = 1;   // number of clients in master area
static const int resizehints = 1;   // 1 means respect size hints in tiled resizals

#include "grid.c"
static const Layout layouts[] = {
  // symbol , function , name
  { "[]="   , tile     , "Tiled"    }, // first entry is default
  { "[M]"   , monocle  , "Monocle"  },
  { "HHH"   , grid     , "Grid"     },
  { "><>"   , NULL     , "Floating" }, // no layout function means floating behavior
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
  { MODKEY                       , KEY , view       , {.ui = 1 << TAG} } , \
  { MODKEY|ControlMask           , KEY , toggleview , {.ui = 1 << TAG} } , \
  { MODKEY|ShiftMask             , KEY , tag        , {.ui = 1 << TAG} } , \
  { MODKEY|ControlMask|ShiftMask , KEY , toggletag  , {.ui = 1 << TAG} } ,

#ifdef CMD
#error "CMD already defined"
#else
#define CMD(...) { .v = (const char*[]){ __VA_ARGS__, NULL } }
#endif

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) CMD("/bin/sh", "-c", cmd)

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
  "dmenu_run",
  "-m", dmenumon,
  "-fn", dmenufont,
  "-nb", col_gray1,
  "-nf", col_gray3,
  "-sb", col_cyan,
  "-sf", col_gray4,
  NULL
};

#include "movestack.c"
#include "shift-tools.c"
static Key keys[] = {
  // modifier          , key          , function         , argument
  { MODKEY             , XK_BackSpace , quit             , {.i = 0 }    },
  { MODKEY|ShiftMask   , XK_BackSpace , quit             , {.i = 1 }    },
  { MODKEY|ShiftMask   , XK_q         , killclient       , {0}          },
  { MODKEY|ShiftMask   , XK_b         , togglebar        , {0}          },
  { MODKEY             , XK_j         , focusstack       , {.i = +1 }   },
  { MODKEY             , XK_k         , focusstack       , {.i = -1 }   },
  { MODKEY|ShiftMask   , XK_j         , movestack        , {.i = +1 }   },
  { MODKEY|ShiftMask   , XK_k         , movestack        , {.i = -1 }   },
  { MODKEY             , XK_h         , setmfact         , {.f = -0.05} },
  { MODKEY             , XK_l         , setmfact         , {.f = +0.05} },
  { MODKEY|ShiftMask   , XK_h         , incnmaster       , {.i = +1 }   },
  { MODKEY|ShiftMask   , XK_l         , incnmaster       , {.i = -1 }   },
  { MODKEY             , XK_m         , focusmaster      , {0}          },
  { MODKEY|ShiftMask   , XK_m         , zoom             , {0}          },
  { MODKEY             , XK_Tab       , view             , {0}          },
  { MODKEY|ShiftMask   , XK_f         , togglefullscr    , {0}          },
  { MODKEY             , XK_space     , togglefloating   , {0}          },
  { MODKEY|ControlMask , XK_space     , setlayout        , {0}          },
  { MODKEY             , XK_0         , view             , {.ui = ~0 }  },
  { MODKEY|ShiftMask   , XK_0         , tag              , {.ui = ~0 }  },
  { MODKEY             , XK_comma     , focusmon         , {.i = -1 }   },
  { MODKEY             , XK_period    , focusmon         , {.i = +1 }   },
  { MODKEY|ShiftMask   , XK_comma     , tagmon           , {.i = -1 }   },
  { MODKEY|ShiftMask   , XK_period    , tagmon           , {.i = +1 }   },
  { MODKEY             , XK_Left      , shiftview        , {.i = -1 }   },
  { MODKEY             , XK_Right     , shiftview        , {.i = +1 }   },
  { MODKEY|ShiftMask   , XK_Left      , shifttag         , {.i = -1 }   },
  { MODKEY|ShiftMask   , XK_Right     , shifttag         , {.i = +1 }   },
  { MODKEY|ShiftMask|ControlMask, XK_Left , shiftboth    , {.i = -1 }   },
  { MODKEY|ShiftMask|ControlMask, XK_Right, shiftboth    , {.i = +1 }   },
  { MODKEY             , XK_Page_Up   , shiftviewclients , {.i = -1 }   },
  { MODKEY             , XK_Page_Down , shiftviewclients , {.i = +1 }   },
  { MODKEY|ShiftMask   , XK_Page_Up   , shifttagclients  , {.i = -1 }   },
  { MODKEY|ShiftMask   , XK_Page_Down , shifttagclients  , {.i = +1 }   },

  { MODKEY|ControlMask , XK_t , setlayout , {.v = &layouts[0]} },
  { MODKEY|ControlMask , XK_m , setlayout , {.v = &layouts[1]} },
  { MODKEY|ControlMask , XK_g , setlayout , {.v = &layouts[2]} },
  { MODKEY|ControlMask , XK_f , setlayout , {.v = &layouts[3]} },

  TAGKEYS(XK_1, 0)  // TAGKEYS(XK_KP_1, 0)
  TAGKEYS(XK_2, 1)  // TAGKEYS(XK_KP_2, 1)
  TAGKEYS(XK_3, 2)  // TAGKEYS(XK_KP_3, 2)
  TAGKEYS(XK_4, 3)  // TAGKEYS(XK_KP_4, 3)
  TAGKEYS(XK_5, 4)  // TAGKEYS(XK_KP_5, 4)
  TAGKEYS(XK_6, 5)  // TAGKEYS(XK_KP_6, 5)
  TAGKEYS(XK_7, 6)  // TAGKEYS(XK_KP_7, 6)
  TAGKEYS(XK_8, 7)  // TAGKEYS(XK_KP_8, 7)
  TAGKEYS(XK_9, 8)  // TAGKEYS(XK_KP_9, 8)

  // Lauch programs
  { MODKEY , XK_p      , spawn , {.v = dmenucmd } },
  // { MODKEY , XK_Return , spawn , CMD("terminal-cwd") },
  { MODKEY , XK_Return , spawn , CMD("alacritty") },
  { MODKEY , XK_w      , spawn , CMD("google-chrome-stable") },
  { MODKEY , XK_d      , spawn , CMD("xreader") },
  { MODKEY , XK_c      , spawn , CMD("gcolor2") },
  { MODKEY , XK_v      , spawn , CMD("pavucontrol") },
  { MODKEY , XK_f      , spawn , CMD("thunar") },
  { MODKEY , XK_s      , spawn , CMD("slack") },
  { MODKEY|ShiftMask , XK_w , spawn , CMD("google-chrome-stable", "--incognito") },

  // dmenu shortcuts
  // google calendar meetings
  { MODKEY , XK_F10  , spawn, CMD("ugcal","--dmenu") },
  // clips
  { MODKEY , XK_slash, spawn, CMD("dmenu_clip") },

  // Shutdown menu
  { 0 , XF86XK_Sleep , spawn , CMD("dmenu_off") },

  // Audio controls
  { 0 , XF86XK_AudioRaiseVolume , spawn , SHCMD(
    // "amixer", "set", "Master", "playback", "5+"
    // "pactl", "set-sink-volume", "0", "+5%"
    "pactl set-sink-volume \"`pactl get-default-sink`\" +5%"
  )},
  { 0 , XF86XK_AudioLowerVolume , spawn , SHCMD(
    // "amixer", "set", "Master", "playback", "5-"
    // "pactl", "set-sink-volume", "0", "-5%"
    "pactl set-sink-volume \"`pactl get-default-sink`\" -5%"
  )},
  { 0 , XF86XK_AudioMute        , spawn , SHCMD(
    // "amixer", "set", "Master", "toggle"
    // "pactl", "set-sink-mute", "0", "toggle"
    "pactl set-sink-mute \"`pactl get-default-sink`\" toggle"
  )},
  { 0 , XF86XK_AudioMicMute     , spawn , SHCMD(
    // "pactl", "set-source-mute", "0", "toggle"
    "pactl set-source-mute \"`pactl get-default-source`\" toggle"
  )},

  // Brightness controls
  { 0 , XF86XK_MonBrightnessUp   , spawn , CMD(
    "/home/ivanp/sys/brightness/brightness", "-u"
  )},
  { 0 , XF86XK_MonBrightnessDown , spawn , CMD(
    "/home/ivanp/sys/brightness/brightness", "-d"
  )},

  // Player controls
  { MODKEY|ControlMask, XK_Down , spawn, CMD("playerctl","play-pause")},
  { MODKEY|ControlMask, XK_Left , spawn, CMD("playerctl","previous")},
  { MODKEY|ControlMask, XK_Right, spawn, CMD("playerctl","next")},

  // Screenshot
#define SCR_CMD(ARGS) SHCMD( \
  "NAME=\"$HOME/Pictures/screenshots/$(date +%s%N).png\";" \
  "maim " ARGS " \"$NAME\";" \
  "echo \"$NAME\" | xclip -r -selection clipboard" \
)
  { 0          , XK_Print, spawn, SCR_CMD("-u"  ) },
  { ControlMask, XK_Print, spawn, SCR_CMD("-uos") },
  { ShiftMask  , XK_Print, spawn, SCR_CMD("-ui $(xdotool getactivewindow)") },
  { Mod1Mask   , XK_Print, spawn, SCR_CMD("-ui $(xdotool selectwindow)") },

  // mount and unmount
  { MODKEY , XK_bracketleft , spawn, CMD( "mnt") },
  { MODKEY , XK_bracketright, spawn, CMD("umnt") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
  // click event     , mask   , button  , function       , argument
  // { ClkStatusText , 0      , Button2 , spawn          , {.v = cmd_term } },
  { ClkLtSymbol      , 0      , Button1 , setlayout      , {0} },
  { ClkLtSymbol      , 0      , Button3 , layoutmenu     , {0} },
  { ClkWinTitle      , 0      , Button2 , zoom           , {0} },
  { ClkClientWin     , MODKEY , Button1 , movemouse      , {0} },
  { ClkClientWin     , MODKEY , Button2 , togglefloating , {0} },
  { ClkClientWin     , MODKEY , Button3 , resizemouse    , {0} },
  { ClkTagBar        , 0      , Button1 , view           , {0} },
  { ClkTagBar        , 0      , Button3 , toggleview     , {0} },
  { ClkTagBar        , MODKEY , Button1 , tag            , {0} },
  { ClkTagBar        , MODKEY , Button3 , toggletag      , {0} },
};
