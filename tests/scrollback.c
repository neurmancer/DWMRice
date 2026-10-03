/* Exercise the actual terminal state without requiring a display. */
#include <assert.h>
#include "../suckless/st/st.c"
unsigned int defaultfg = 258, defaultbg = 259, defaultcs = 256;
unsigned int tabspaces = 8;
wchar_t *worddelimiters = L" ";
int allowaltscreen = 1, allowwindowops = 0;
char *vtiden = "", *termname = "st-256color", *utmp = NULL, *scroll = NULL, *stty_args = "";
void xsetmode(int a, unsigned int b) { (void)a; (void)b; }
void xsetpointermotion(int a) { (void)a; }
void xsettitle(char *a) { (void)a; }
void xseticontitle(char *a) { (void)a; }
void xloadcols(void) {}
void xbell(void) {}
void xsetsel(char *a) { free(a); }
void xclipcopy(void) {}
int xsetcursor(int a) { (void)a; return 0; }
int xsetcolorname(int a, const char *b) { (void)a; (void)b; return 0; }
int xgetcolor(int a, unsigned char *b, unsigned char *c, unsigned char *d) { (void)a; (void)b; (void)c; (void)d; return 1; }
int xstartdraw(void) { return 0; }
void xdrawline(Line a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
void xdrawcursor(int a, int b, Glyph c, int d, int e, Glyph f) { (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; }
void xfinishdraw(void) {}
void xximspot(int a, int b) { (void)a; (void)b; }

int main(void) {
    selinit();
    tnew(20, 5);
    kscrollup(&(Arg){.i=100});
    assert(term.scr == 0);
    for (int i=0; i<12; i++) {
        char text[32];
        snprintf(text, sizeof text, "ROW%02d\r\n", i);
        twrite(text, strlen(text), 0);
    }
    assert(term.histlen == 8);
    kscrollup(&(Arg){.i=100});
    assert(term.scr == 8);
    assert(TLINE(0)[0].u == 'R' && TLINE(0)[4].u == '0');
    selstart(0, 0, 0);
    selextend(4, 0, SEL_REGULAR, 0);
    selextend(4, 0, SEL_REGULAR, 1);
    char *selection = getsel();
    assert(selection && !strncmp(selection, "ROW00", 5));
    free(selection);
    selclear();
    kscrolldown(&(Arg){.i=100});
    assert(term.scr == 0);
    int saved = term.histlen;
    twrite("\033[?1049h", 8, 0);
    for (int i=0; i<10; i++) twrite("ALT\r\n", 5, 0);
    kscrollup(&(Arg){.i=3});
    assert(term.scr == 0 && term.histlen == saved);
    twrite("\033[?1049l", 8, 0);
    kscrollup(&(Arg){.i=3});
    assert(term.scr == 3);
    tresize(40, 10);
    assert(term.histlen == saved);
    tresize(10, 3);
    for (int i=0; i<2100; i++) twrite("WRAP\r\n", 6, 0);
    assert(term.histlen == HISTSIZE);
    kscrollup(&(Arg){.i=5000});
    assert(term.scr == HISTSIZE);
    treset();
    assert(term.histlen == 0 && term.scr == 0);
    puts("PASS: scrollback bounds, selection, alternate screen, resize, ring wrap, reset");
    return 0;
}
