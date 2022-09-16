/*** includes ***/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/*** Data ***/
struct termios orig_termios;

/*** Terminal ***/
void die(const char *s){
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

/*** Init ***/
int main(){
	enableRawMode();

	while(1){
		char c = '\0';
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read"); 
		if(iscntrl(c)) {
			printf("%d\n", c);
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
		if(c == 'q') break;
	}

	system("clear"); // clear terminal after running for QOL
	return 0;
}
