#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <X11/Xlib.h>
#include <linux/limits.h>
#include <linux-keys.h>

#define SPAWN_AND_DIE(xpid) \
	if((xpid=fork()) < 0) \
		return FALSE; \
	else if(xpid > 0) \
		exit(0)

#define VCALL_LZ(callback,...) \
	if(callback(__VA_ARGS__) < 0) \
		return FALSE

#define CALL_LZ(callback) \
	if(callback() < 0) \
		return FALSE

#define UNUSED_PARAM(xparam) \
	((void)xparam)

#define GUESS_MAX_FD 1024
#define OPTIONS 4
#define MASK 0xFFFFE7FCL

typedef struct __opt {
	char desc[17];
	char name[2];
	char fill_zero;
} kbledctl_opt;

typedef enum __bool {
	FALSE,
	TRUE
} kbledctl_bool;

static Display* d = NULL;
static FILE* f = NULL;

static kbledctl_opt options[OPTIONS] = {
	{ "print this help",   "-h" , 0 },
	{ "change display",    "-d" , 0 },
	{ "don't daemonize",   "-n" , 0 },
	{ "change led number", "-l" , 0 }
};

void cleanupexit()
{
	if(d) {
		XFlush(d);
		XCloseDisplay(d);
	}
	
	if(f)
		fclose(f);
	
	exit(0);
}

static void cleanup_and_exit(int sig)
{
	UNUSED_PARAM(sig);
	cleanupexit();
}

kbledctl_bool make_daemon() 
{
	pid_t newpid;

	umask(0);

	SPAWN_AND_DIE(newpid);
	CALL_LZ(setsid);
	SPAWN_AND_DIE(newpid);
	
	VCALL_LZ(chdir,"/");
 
	for(int fd=0;fd<=GUESS_MAX_FD;fd++)
		close(fd);

	VCALL_LZ(open,"/dev/null",O_RDWR);

	VCALL_LZ(dup,0);
	VCALL_LZ(dup,0);

	return TRUE;
}

void listen_events(const char* display, const int nled) 
{
	char target[PATH_MAX];
	if(get_keyboard_evpath_s(target) != 0)
		exit(1);

	d = XOpenDisplay(display);
	if(d == NULL)
		exit(1);
	
	key press;
	f = fopen(target,"r");
	if(f == NULL)
		exit(1);

	XKeyboardState current_state;
	XKeyboardControl ctl;

	memset(&ctl,0,sizeof(XKeyboardControl));
	memset(&current_state,0,sizeof(XKeyboardState));

	ctl.led = nled;

	signal(SIGINT,cleanup_and_exit);
	signal(SIGTERM,cleanup_and_exit);

	while(TRUE) {
		press = wait_for_keypress(f);
	   if(press == KEY_SCROLLLOCK) {
	   	XGetKeyboardControl(d,&current_state);
			ctl.led_mode = !(MASK == (current_state.led_mask & MASK));
			XChangeKeyboardControl(d,KBLedMode,&ctl);
			XFlush(d);
		}
	}
}

void print_help_exit(char* execname) 
{
	printf("usage: %s [-hn] [-d XDISPLAY] [-l LED_INT]\n",execname);
	for(int i=0;i<OPTIONS;i++)
		printf(" %s : %s\n",options[i].name, options[i].desc);
	exit(0);
}

int main(int argc, char** argv)
{
	if(getuid() > 0) {
		fputs("privileged user only.",stderr);
		return 1;
	}

	kbledctl_bool daemonize = TRUE;
	int target_led = 3;
	char* target_display_server = NULL;
	char* end = NULL; //used for strtol 

	int current;
	while((current=getopt(argc,argv,"nhd:l:")) != -1) {
		switch(current) {
			case 'n':
				daemonize = FALSE;
				break;
			case 'l':
            errno = 0;
            target_led = (int) strtol(optarg,&end,10);
            if(errno) {
            	perror("unable to get integer (strtol):");
            	return 1;
            }
				break;
			case 'd':
				target_display_server = optarg;
				break;
			case 'h':
				print_help_exit(argv[0]);
				break;
			default:
				exit(1);
		}
	}
	
	if(daemonize) {
		if(!make_daemon()) {
			fprintf(stderr,"cannot make daemon process, aborting...\n");
			return 1;
		}
	}

	listen_events(target_display_server,target_led);

	return 0;
}
