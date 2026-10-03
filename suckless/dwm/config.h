/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int barpadding = 18;     /* extra bar height around text */
static const char barbadge[] = "NSD // CTRL";
static const int extrabar           = 1;        /* 0 means no extra bar */
static const char *fonts[]          = { "DejaVu Sans Mono:size=9" };
static const char dmenufont[]       = "DejaVu Sans Mono:size=10";
static const char col_gray1[]       = "#090D16";
static const char col_gray2[]       = "#27334B";
static const char col_gray3[]       = "#5A96FF";
static const char col_gray4[]       = "#090D16";
static const char default_flycolor[] = "#C170FF";

/* NSD signal palette: violet, blue, green, amber. */
static const char *flycolors[] = {
    "#C170FF", "#5A96FF", "#00EB74", "#FFB85C", NULL
};

static const char *colors[][3] = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeTitle] = { "#C170FF", "#181426", "#C170FF" },
	[SchemeDim]   = { "#65748F", "#090D16", "#27334B" },
	[SchemeBlue]  = { "#5A96FF", "#101A2B", "#5A96FF" },
	[SchemeGreen] = { "#00EB74", "#0D211C", "#00EB74" },
	[SchemeAlert] = { "#FFB85C", "#2A1C19", "#FFB85C" },
	[SchemeSel]  = { col_gray4, default_flycolor, default_flycolor }, /* [1] and [2] are changed in cycle_flycolors */
};

/* tagging */
static const char *tags[] = { "01 SYS", "02 DEV", "03 NET", "04 WRK", "05 AUX", "06", "07", "08", "09" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 2,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
 	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
	"dmenu_run",
	"-m", dmenumon,
	"-fn", dmenufont,
	"-nb", col_gray1,
	"-nf", col_gray3,
	"-sb", default_flycolor, /* changed as dmenucmd[10] in cycle_flycolors */
	"-sf", col_gray4,
	NULL
};
static const char *termcmd[]  = { "st", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_grave,  togglescratch,  {0} },
	{ MODKEY,                       XK_minus,  changegap,      {.i = -2} },
	{ MODKEY,                       XK_equal,  changegap,      {.i = +2} },
	{ MODKEY,                       XK_g,      togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("rice-menu") },
	{ MODKEY|ControlMask,           XK_l,      spawn,          SHCMD("rice-lock") },
	{ 0,                            XK_Print,  spawn,          SHCMD("rice-shot screen") },
	{ ShiftMask,                    XK_Print,  spawn,          SHCMD("rice-shot region") },
	{ MODKEY,                       XK_Print,  spawn,          SHCMD("rice-shot window") },
	{ MODKEY|ShiftMask,             XK_s,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ShiftMask,             XK_d,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_c,      cycle_flycolors, { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_v,      cycle_flycolors, { .i = -1 } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_b,      toggleextrabar, {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_x,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      spawn,          SHCMD("rice-menu") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	{ ClkBadge,            0,              Button1,        spawn,          SHCMD("rice-menu") },
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

