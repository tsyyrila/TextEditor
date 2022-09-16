/*** includes ***/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/*** Defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)

/*** Data ***/
struct editorConfig{
	struct termios orig_termios;
};

struct editorConfig E;

/*** Terminal ***/
void die(const char *s){
	write(STDOUT_FILENO, "\x1b[2j",4);
	write(STDOUT_FILENO, "\x1b[h",3);

	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
	die("tcsetattr");
}

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcsetattr");
	atexit(disableRawMode);
	
	struct termios raw = orig_termios;
	raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP |IXON | ICRNL);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)die("tcsetattr");
}

char editorReadKey(){
	int nread;
	char c;
	while((nread = read(STDIN_FILENO, &c, 1))!=1){
		if (nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
	}

/*** Input ***/
void editorProcessKeypress(){
	char c = editorReadKey();

	switch (c){
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2j",4);
			write(STDOUT_FILENO, "\x1b[h",3);
			exit(0);
			break;
	}
}

/*** Output ***/
void editorDrawRows(){
	int y;
	for(y=0; y<24; y++){
		write(STDOUT_FILENO, "~\r\n",3);
	}
}

void editorRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2j",4);
	write(STDOUT_FILENO, "\x1b[h",3);

	editorDrawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** Init ***/
int main(){
	enableRawMode();

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}

	system("clear"); // clear terminal after running for QOL
	return 0;
}
