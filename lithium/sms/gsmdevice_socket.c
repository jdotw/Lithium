#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>

#include "sms_config.h"

i_socket* l_gsmdevice_socket_open (i_resource *self)
{
  i_socket *sock;
  struct termios termio;
  l_sms_config *config;

  sock = i_socket_create ();
  if (!sock)
  { i_printf (1, "l_gsmdevice_socket_open failed to create socket struct"); return NULL; }

  config = l_sms_config_file_to_struct (self);
  if (!config)
  { i_printf (1, "l_gsmdevice_socket_open failed to load config"); return NULL; }

  sock->sockfd = open(config->dev_str, O_RDWR | O_NOCTTY ); 
  if (sock->sockfd <0) 
  { 
    i_printf (1, "l_gsmdevice_socket_open failed to open %s (%s)", config->dev_str, strerror(errno));
    l_sms_config_free (config);
    return NULL;
  }
  l_sms_config_free (config);
  
  memset (&termio, 0, sizeof(struct termios)); /* clear struct for new port settings */

  /* 
   * BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
   * CRTSCTS : output hardware flow control (only used if the cable has
   *           all necessary lines. See sect. 7 of Serial-HOWTO)
   * CS8     : 8n1 (8bit,no parity,1 stopbit)
   * CLOCAL  : local connection, no modem contol
   * CREAD   : enable receiving characters
   */

  termio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
         
  /*
   * IGNPAR  : ignore bytes with parity errors
   * ICRNL   : map CR to NL (otherwise a CR input on the other computer
   *           will not terminate input) otherwise make device raw 
   *           (no other input processing)
   */

  termio.c_iflag = IGNPAR | ICRNL;
         
  /*
   * Raw output.
   */
  
  termio.c_oflag = 0;
         
  /*
   * ICANON  : enable canonical input disable all echo functionality, 
   * and don't send signals to calling program
   */

  termio.c_lflag = ICANON;
         
  /*
   * initialize all control characters
   * default values can be found in /usr/include/termios.h, and are given
   * in the comments, but we don't need them here
   */

  termio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  termio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  termio.c_cc[VERASE]   = 0;     /* del */
  termio.c_cc[VKILL]    = 0;     /* @ */
  termio.c_cc[VEOF]     = 4;     /* Ctrl-d */
  termio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
  termio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
//  termio.c_cc[VSWTC]    = 0;     /* '\0' */   /* FIX Not defined in OSX */
  termio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
  termio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
  termio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
  termio.c_cc[VEOL]     = 0;     /* '\0' */
  termio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
  termio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
  termio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
  termio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
  termio.c_cc[VEOL2]    = 0;     /* '\0' */
        
  /*
   * now clean the modem line and activate the settings for the port
   */

  tcflush(sock->sockfd, TCIFLUSH);
  tcsetattr(sock->sockfd,TCSANOW,&termio);

  return sock;
}
