/*****************************************************************************/
/*                                                                           */
/* Module: seru.c                                                            */
/*                                                                           */
/* Description: Common functions to use serial ports                         */
/*                                                                           */
/* Copyright (C) 2004, 2010, 2011  David Wilson Clarke                       */
/*                                                                           */
/* This file is part of Seru-p.                                              */
/*                                                                           */
/* Seru-p is free software; you can redistribute it and/or modify            */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* any later version.                                                        */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*      INCLUDED FILES (dependencies)                                        */
/*****************************************************************************/

/* Required to include unistd.h */
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>       /* Standard I/O definitions  - fopen()            */
#include <unistd.h>      /* UNIX standard function definitions - close()   */
                         /* getpid()                                       */
#include <string.h>      /* Standard string lib - memmove(), strlen(),     */
                         /* strrchr(), strcpy(), strcat                    */
#include <errno.h>       /* Um, error numbers - errno                      */
#include <fcntl.h>       /* File control definitions - open(), fcntl()     */
#include <termios.h>     /* Standard input/output definitions              */
#include <sys/select.h>  /* Select stuff - select()                        */
#include <stdbool.h>     /* Boolean types                                  */
#include <sys/time.h>    /* Standard time definitions - gettimeofday()     */
#include <sys/ioctl.h>   /* ioctl() stuff                                  */
#include <linux/serial.h>
                         /* Serial stuff - ASYNC_LOW_LATENCY, serial_struct*/
#include "seru.h"        /* Header file for the serial utils library       */


/*****************************************************************************/
/*      INTERNAL MACRO DEFINITIONS                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Name:                                                                    */
/*  Description:                                                             */
/*  Parameters:                                                              */
/*  Client functions:                                                        */
/*****************************************************************************/


/*****************************************************************************/
/*      INTERNAL TYPE DEFINITIONS                                            */
/*****************************************************************************/

/* Types */

/* Port lock state*/
typedef enum lock_state_t {i_UNLOCKED, i_LOCKED} lock_state_t;


/* Enums */

enum { i_RX_BUF_SIZE = 30 }; /* Length of the RX buffer            */


/* String literals */

/* Default path for the lock file */
static const char *i_DEFAULT_LOCK_PATH = "/var/lock/";

/* Default lock file extension */
static const char *i_DEFAULT_LOCK_EXT = "LCK..";


/* Structures */

/* Structure to hold baud rate data */
struct i_baud_data
{
  unsigned long value;    /* The baud rate as a number              */
  speed_t speed;          /* The defined constant for the baud rate */
  char *str;              /* The baud rate as a string              */
  seru_timeout_t timeout; /* The timeout time for the baud rate     */
};


/*****************************************************************************/
/*      INTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/

static struct termios i_saved_options;
                                     /* Structure for the saved port options */

static bool i_use_parity;            /* Is parity being used                 */


/* Table to hold baud rate data */

/* Timeout taken from rounded doubled observed maximums */
/* Timeout in microseconds                              */

static const struct i_baud_data i_baud_tab[] =
{

 /* Value   Constant, String,   Timeout */
  { 50,     B50,      "50",     500000     },
  { 75,     B75,      "75",     500000     },
  { 110,    B110,     "110",    500000     },
  { 134,    B134,     "134",    300000     },
  { 150,    B150,     "150",    200000     },
  { 200,    B200,     "200",    300000     },
  { 300,    B300,     "300",    200000     },
  { 600,    B600,     "600",    500000     },
  { 1200,   B1200,    "1200",   200000     },
  { 1800,   B1800,    "1800",   400000     },
  { 2400,   B2400,    "2400",   200000     },
  { 4800,   B4800,    "4800",   150000     },
  { 9600,   B9600,    "9600",   300000     },
  { 19200,  B19200,   "19200",  300000     },
  { 38400,  B38400,   "38400",  200000     },
  { 57600,  B57600,   "57600",  200000     },
  { 115200, B115200,  "115200", 300000     },
  {      0,       0,        "",        0   }  /* Marks end of table */

};


/*****************************************************************************/
/*      INTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/* Name: i_get_lock_file_name()                                              */
/*                                                                           */
/* Description: Works out the port lock file name from the port name         */
/*                                                                           */
/* Uses: lock_file_name - The port lock file name string                     */
/*       lock_file - Pointer to the buffer to return the port lock file      */
/*       name and path                                                       */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

void i_get_lock_file_name(char *port_name, char *lock_file_name)
{

  size_t  lock_file_name_len = 0; /* Lock file name length                  */
  char *lock_file_name_ptr;       /* Ptr to the lock file name in port name */


  /* Find the port name, without the path */
  /* Look for last occurrence of '/'      */
  lock_file_name_ptr = strrchr(port_name, '/');

  /* If '/' wasn't found... */
  if(lock_file_name_ptr == NULL)
  {

    /* No '/' found so no path to strip off */
    lock_file_name_ptr = port_name;

  }
  else
  {

    /* Point after '/' */
    lock_file_name_ptr++;

  }

  lock_file_name_len = strlen(lock_file_name_ptr);

  (void) memmove(lock_file_name, lock_file_name_ptr,
    (lock_file_name_len + 1) );

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_get_lock_name()                                                   */
/*                                                                           */
/* Description: Works out the port lock file name and path                   */
/*                                                                           */
/* Uses: The port lock file name string                                      */
/*       Pointer to the buffer to return the port lock file name and path    */
/*                                                                           */
/* Returns: Void                                                             */
/*                                                                           */
/*****************************************************************************/

void i_get_lock_name(char *lock_file_name, char *lock_file)
{

  /* char *lock_file_path_ptr; */


  /* Use the following two active lines and comment out the third line if a */
  /* lock file path environmental variable is used. LOCK_PATH is not the    */
  /* true name, just a place marker for a real one                          */

  /* Start by adding the lock file path */
  /* lock_file_path_ptr = getenv(LOCK_PATH); */

  /* (void) strcpy(lock_file, lock_file_path_ptr); */

  /* Start by adding the lock file path */
  (void) strcpy(lock_file, i_DEFAULT_LOCK_PATH);

  /* Add the lock file name extension to the file name */
  (void) strcat(lock_file, i_DEFAULT_LOCK_EXT);

  /* Add the file name to the path, giving us the full name */
  (void) strcat(lock_file, lock_file_name);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_for_lock()                                                  */
/*                                                                           */
/* Description: Checks if a port is locked                                   */
/*                                                                           */
/* Uses: lock_file - The port lock file name string                          */
/*                                                                           */
/* Returns: The current lock state                                           */
/*                                                                           */
/*****************************************************************************/

lock_state_t i_check_for_lock(char *lock_file)
{

  FILE *lock_file_ptr = (FILE *)0;     /* Pointer to the file opened         */
  lock_state_t port_state = i_LOCKED;  /* Flag showing if port locked or not */
  int lock_process_no = 0;             /* PID obtained from lock file        */
  pid_t lock_pid = 0;                  /* Obtained PID as PID type           */
  pid_t lock_sid = 0;                  /* Returned SID flagging running proc */


  /* Attempt to open an existing lock file */
  lock_file_ptr = fopen(lock_file, "r");

  if(lock_file_ptr == NULL)
  {

    /* Unable to open file. This could be because there is no lock file */
    /* or we are not allowed access to it. Assume it does not exist.    */
    /* We will find if access is not allowed later.                     */
    port_state = i_UNLOCKED;

  }
  else
  {

    /* Was the lock file created by a process which is still running */

    /* read in process number */
    (void) fscanf(lock_file_ptr, "%d", &lock_process_no);

    /* Have we a valid process number? */
    if(lock_process_no > 0)
    {

      lock_pid = (pid_t) lock_process_no;

      lock_sid = getsid(lock_pid);

      /* Is the process still running */
      if(lock_sid == -1)
      {

        port_state = i_UNLOCKED;

      }
      else
      {

        port_state = i_LOCKED;

      }

    }
    else
    {

      /* Invalid process number, so assume the port is unlocked */
      port_state = i_UNLOCKED;

    }

    /* Close the file. Discard result, as, if it fails, what can we do? */
    (void) fclose(lock_file_ptr);

  }

  return port_state;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_create_lock_file()                                                */
/*                                                                           */
/* Description: Locks a serial port by creating a lock file                  */
/*                                                                           */
/* Uses: lock_file - The port lock file name string                          */
/*                                                                           */
/* Returns: The current lock state                                           */
/*                                                                           */
/*****************************************************************************/

lock_state_t i_create_lock_file(char *lock_file)
{

  FILE *lock_file_ptr = (FILE *)0;     /* Pointer to the file opened         */
  lock_state_t port_state = i_LOCKED;  /* Flag showing if port locked or not */
  pid_t lock_pid = 0;                  /* Obtained PID as PID type           */
  int fclose_return = 0;               /* Flag returned from fclose          */


  /* Create and open the lock file, if we can */
  lock_file_ptr = fopen(lock_file, "w");

  if(lock_file_ptr == NULL)
  {

    port_state = i_UNLOCKED;

  }
  else
  {

    /* Write our own process number to the lock file */
    lock_pid = getpid();

    fprintf(lock_file_ptr, "%10d\n", (int) lock_pid);

    port_state = i_LOCKED;

    /* Close the file */
    fclose_return = fclose(lock_file_ptr);

    if(fclose_return == EOF)
    {

      /* Close failed, we can't guarantee the lock file is created */
      port_state = i_UNLOCKED;

    }

  }

  return port_state;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_data_bits()                                              */
/*                                                                           */
/* Description: Set the number of data bits for a serial ports               */
/*                                                                           */
/* Uses: flag - A pointer to the c_cflag to modify                           */
/*       data_bits - The required data bits                                  */
/*                                                                           */
/* Returns: Status: OK or invalid                                            */
/*                                                                           */
/*****************************************************************************/

static int i_set_port_data_bits(tcflag_t *flag, seru_data_bits_t data_bits)
{

  int status = 0; /* Status: OK or invalid */


  /* Clear data size bits */
  *flag &= ~CSIZE;

  switch(data_bits)
  {

    case SERU_5_DATA_BITS:

      /* Set 5 data bits */
      *flag |= CS5;

      break;

    case SERU_6_DATA_BITS:

      /* Set 6 data bits */
      *flag |= CS6;

      break;

    case SERU_7_DATA_BITS:

      /* Set 7 data bits */
      *flag |= CS7;

      break;

    case SERU_8_DATA_BITS:

      /* Set 8 data bits */
      *flag |= CS8;

      break;

    default:

      /* Invalid data bits */
      status = SERU_STP_BAD_DATA_BITS;

  }

  return status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_parity()                                                 */
/*                                                                           */
/* Description: Set the parity for a serial port                             */
/*                                                                           */
/* Uses: flag - A pointer to the c_cflag to modify,                          */
/*       parity - The required parity                                        */
/*                                                                           */
/* Returns: Status: OK or invalid                                            */
/*                                                                           */
/*****************************************************************************/

static int i_set_port_parity(tcflag_t *flag, seru_parity_t parity)
{

  int status = 0; /* Status: OK or invalid */


  switch(parity)
  {

    case SERU_ODD_PARITY:

      /* Enable parity */
      *flag |= PARENB;

      /* Set odd parity */
      *flag |= PARODD;

      /* Not parity is being used */
      i_use_parity = true;

      break;

    case SERU_EVEN_PARITY:

      /* Enable parity */
      *flag |= PARENB;

      /* Disable odd parity */
      *flag &= ~PARODD;

      /* Not parity is being used */
      i_use_parity = true;

      break;

    case SERU_NO_PARITY:

      /* Disable parity */
      *flag &= ~PARENB;

      /* Not parity is not being used */
      i_use_parity = false;

      break;

    default:

      /* Invalid parity */
      status = SERU_STP_BAD_PARITY;

  }

  return status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_stop_bits()                                              */
/*                                                                           */
/* Description: Set the stop bits for a serial port                          */
/*                                                                           */
/* Uses: flag - A pointer to the c_cflag to modify                           */
/*       stop_bits - The required stop bits                                  */
/*                                                                           */
/* Returns: Status: OK or invalid                                            */
/*                                                                           */
/*****************************************************************************/

static int i_set_port_stop_bits(tcflag_t *flag, seru_stop_bits_t stop_bits)
{

  int status = 0; /* Status: OK or invalid */


  switch(stop_bits)
  {

    case SERU_1_STOP_BIT:

      /* Disable 2 stop bits */
      *flag &= ~CSTOPB;

      break;

    case SERU_2_STOP_BIT:

      /* Set 2 stop bits */
      *flag |= CSTOPB;

      break;

    default:

      /* Invalid stop bits */
      status = SERU_STP_BAD_STOP_BITS;

  }

  return status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_iflag()                                                  */
/*                                                                           */
/* Description: Set the iflag config of a serial port                        */
/*                                                                           */
/* Uses: flag - A pointer to the c_iflag to modify                           */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_iflag(tcflag_t *flag)
{

  /* Disable ignore break - Not set */
  *flag &= ~IGNBRK;

  /* Disable SIGINT on break - Not set */
  *flag &= ~BRKINT;

  /* Disable ignore parity and framing errors - Not set */
  *flag &= ~IGNPAR;

  /* Mark parity and framing errors - Set */
  *flag |= PARMRK;

  /* Disable input parity checking - Not set */
  *flag &= ~INPCK;

  /* Disable stripping of 8th bit - Not set */
  *flag &= ~ISTRIP;

  /* Disable NL to CR translation - Not set */
  *flag &= ~INLCR;

  /* Disable ignore CR on input - Not set */
  *flag &= ~IGNCR;

  /* Disable translation of CR to NL on input - Not set */
  *flag &= ~ICRNL;

  /* Disable XON/XOFF on output - Not set */
  *flag &= ~IXON;

  /* Disable XON/XOFF on input - Not set */
  *flag &= ~IXOFF;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_oflag()                                                  */
/*                                                                           */
/* Description: Set the oflag config of a serial port                        */
/*                                                                           */
/* Uses: flag - A pointer to the c_oflag to modify                           */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_oflag(tcflag_t *flag)
{

  /* Disable output processing - Not set */
  *flag &= ~OPOST;

  /* Disable translation of NL to CR-NL - Not set */
  *flag &= ~ONLCR;

  /* Disable translation of CR to NL - Not set */
  *flag &= ~OCRNL;

  /* Disable don't output CR at column zero - Not set */
  *flag &= ~ONOCR;

  /* Disable don't output CR - Not set */
  *flag &= ~ONLRET;

  /* Timed delay, not fill characters - Not set */
  *flag &= ~OFILL;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_cflag()                                                  */
/*                                                                           */
/* Description: Set the cflag config of a serial port                        */
/*                                                                           */
/* Uses: flag - A pointer to the c_cflag to modify                           */
/*       data - The number of data bits                                      */
/*       parity - The parity                                                 */
/*       stop - The number of stop bits                                      */
/*                                                                           */
/* Returns: Status: OK or invalid                                            */
/*                                                                           */
/*****************************************************************************/

static int i_set_port_cflag(tcflag_t *flag, seru_data_bits_t data,
  seru_parity_t parity, seru_stop_bits_t stop)
{

  int status = 0; /* Status: OK or invalid */

  /* Set data bits */
  status |= i_set_port_data_bits(flag, data);

  /* Set stop bits */
  status |= i_set_port_stop_bits(flag, stop);

  /* Enable the receiver - Set */
  *flag |= CREAD;

  /* Set parity */
  status |= i_set_port_parity(flag, parity);

  /* Lower control lines on close - Set */
  *flag |= HUPCL;

  /* Set local mode - Set */
  *flag |= CLOCAL;

  return status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_lflag()                                                  */
/*                                                                           */
/* Description: Set the lflag config of a serial port                        */
/*                                                                           */
/* Uses: flag - A pointer to the c_lflag to modify                           */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_lflag(tcflag_t *flag)
{

  /* Ignore special bytes - Not set */
  *flag &= ~ISIG;

  /* Disable canonical mode - Not set */
  *flag &= ~ICANON;

  /* Disable echo - Not set */
  *flag &= ~ECHO;

  /* Disable echo erase - Not set */
  *flag &= ~ECHOE;

  /* Disable echo NL after kill - Not set */
  *flag &= ~ECHOK;

  /* Disable echo NL - Not set */
  *flag &= ~ECHONL;

  /* Enable flushing the I/O queues when generating signals - Not set */
  *flag &= ~NOFLSH;

  /* Disable SIGTTOU - Not set */
  *flag &= ~TOSTOP;

  /* Disable extended functions - Not set */
  *flag &= ~IEXTEN;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_c_cc()                                                   */
/*                                                                           */
/* Description: Set the c_cc config of a serial port                         */
/*                                                                           */
/* Uses: c_cc_array - A pointer to the c_cc to modify                        */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_c_cc(cc_t c_cc_array[])
{

  /* Set interrupt char to ETX */
  c_cc_array[VINTR] = (cc_t) 3;

  /* Set quit char to FS */
  c_cc_array[VQUIT] = (cc_t) 34;

  /* Set erase char to BS */
  c_cc_array[VERASE] = (cc_t) 8;

  /* Set kill char to NAK */
  c_cc_array[VKILL] = (cc_t) 21;

  /* Set EOF char to EOT */
  c_cc_array[VEOF] = (cc_t) 4;

  /* Set VMIN to 1 char */
  c_cc_array[VMIN] = (cc_t) 1;

  /* Set EOL char to NUL */
  c_cc_array[VEOL] = (cc_t) 0;

  /* Set VTIME to 100mS */
  c_cc_array[VTIME] = (cc_t) 1;

  /* Set VSTART to XON */
  c_cc_array[VSTART] = (cc_t) 17;

  /* Set VSTOP to XOFF */
  c_cc_array[VSTOP] = (cc_t) 19;

  /* Set VSUSP to 0 */
  c_cc_array[VSUSP] = (cc_t) 0;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_get_port_attribs()                                                */
/*                                                                           */
/* Description: Get the port attributes                                      */
/*                                                                           */
/* Uses: setup - A pointer to the setup & status for the port                */
/*       options - A pointer to the port options                             */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_get_port_attribs(struct seru_port_setup_t *setup,
  struct termios *options)
{

  int getattr_return = SERU_PORT_SUCCESS;


  /* Get the current options for the port */
  getattr_return = tcgetattr(setup->fd, options);

  /* Was tcgetattr successful? */
  if(getattr_return == SERU_PORT_FAILURE)
  {

    setup->status |= SERU_STP_GET_OPTS_FAIL;

    /* Report errno */
    setup->setup_errno = errno;

  }

  return getattr_return;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_input_speed()                                            */
/*                                                                           */
/* Description: Set the port input speed                                     */
/*                                                                           */
/* Uses: setup - A pointer to the setup & status for the port                */
/*       options - A pointer to the port options                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_input_speed(struct seru_port_setup_t *setup,
  struct termios *options)
{

  int cfsetispeed_return = SERU_PORT_SUCCESS;
                            /* Value returned by set i/p speed function  */


  /* Set the input baud rate */
  cfsetispeed_return = cfsetispeed(options, setup->speed);

  /* Was cfsetispeed successful? */
  if(cfsetispeed_return == SERU_PORT_FAILURE)
  {

    setup->status |= SERU_STP_IBAUD_FAIL;

    /* Report errno */
    setup->setup_errno = errno;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_output_speed()                                           */
/*                                                                           */
/* Description: Set the port output speed                                    */
/*                                                                           */
/* Uses: setup - A pointer to the setup & status for the port                */
/*       options - A pointer to the port options                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_output_speed(struct seru_port_setup_t *setup,
  struct termios *options)
{

  int cfsetospeed_return = SERU_PORT_SUCCESS;
                            /* Value returned by set o/p speed function  */


  /* Set the output baud rate */
  cfsetospeed_return = cfsetospeed(options, setup->speed);

  /* Was cfsetospeed successful? */
  if(cfsetospeed_return == SERU_PORT_FAILURE)
  {

    setup->status |= SERU_STP_OBAUD_FAIL;

    /* Report errno */
    setup->setup_errno = errno;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_options()                                                */
/*                                                                           */
/* Description: Set the port options                                         */
/*                                                                           */
/* Uses: setup - A pointer to the setup & status for the port                */
/*       options - A pointer to the port options                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_set_port_options(struct seru_port_setup_t *setup,
  struct termios *options)
{

  int tcsetattr_return = SERU_PORT_SUCCESS;
                            /* Value returned by set attributes function */


  /* Set the serial port cflag */
  setup->status |= i_set_port_cflag(&(options->c_cflag), setup->data_bits,
    setup->parity, setup->stop_bits);

  /* Set the serial port iflag */
  i_set_port_iflag(&(options->c_iflag));

  /* Set the serial port oflag */
  i_set_port_oflag(&(options->c_oflag));

  /* Set the serial port lflag */
  i_set_port_lflag(&(options->c_lflag));

  /* Set the serial port c_cc */
  i_set_port_c_cc(options->c_cc);

  /* Set the new options for the port */
  tcsetattr_return = tcsetattr(setup->fd, TCSANOW, options);
  /* Warning - success returned if any set attributes were successful  */
  /* Check that attributes have been set with a tcgetattr              */

  /* Was tcsetattr successful? */
  if(tcsetattr_return == SERU_PORT_FAILURE)
  {

    setup->status |= SERU_STP_SET_OPTS_FAIL;

    /* Report errno */
    setup->setup_errno = errno;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_low_latency()                                                 */
/*                                                                           */
/* Description: Set the port to use low latency                              */
/*                                                                           */
/* Uses: setup - A pointer to the setup & status for the port                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/
static void i_set_low_latency(struct seru_port_setup_t *setup)
{

  struct serial_struct ioctlflags;  /* Flags returned by ioctl      */
  int ioctl_return;                 /* Status returned by ioctl     */


  if(setup->low_latency == true)
  {

    /* Set low latency. First get exising flags */ 
    ioctl_return = ioctl (setup->fd, TIOCGSERIAL, &ioctlflags);

    /* Did the flag fetch go well? */
    if(ioctl_return == SERU_PORT_FAILURE)
    {

      /* Could not get ioctl flags */
      setup->status |= SERU_STP_IOCTL_GET_FAIL;

      /* Report errno  */
      setup->setup_errno = errno;

    }
    else
    {

      /* Set the low latency flag */
      ioctlflags.flags |= ASYNC_LOW_LATENCY;


      /* Write the flag */
      ioctl_return = ioctl (setup->fd, TIOCSSERIAL, &ioctlflags);

      /* Did the flag write go well? */
      if(ioctl_return == SERU_PORT_FAILURE)
      {

        /* Could not set ioctl flags */
        setup->status |= SERU_STP_IOCTL_SET_FAIL;

        /* Report errno  */
        setup->setup_errno = errno;
        
      }
      
    }

  }

}


/*****************************************************************************/
/*      EXTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/


/*****************************************************************************/
/*      EXTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_timeout()                                                  */
/*                                                                           */
/* Description: Gets the timeout for the given baud rate                     */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used: i_baud_tab                                       */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   baud            speed_t       The baud rate                             */
/*                                                                           */
/* Returns: The timeout, or a failure indication                             */
/*                                                                           */
/*****************************************************************************/

extern seru_timeout_t seru_get_timeout(speed_t baud)
{

  int baud_table_index = 0;                      /* Loop counter */
  seru_timeout_t timeout = SERU_GET_TIMEOUT_FAIL; /* The timeout  */


  /* Loop round the baud rate table */
  while(i_baud_tab[baud_table_index].value != 0)
  {

    /* Found a matching baud rate */
    if(baud == i_baud_tab[baud_table_index].speed)
    {

      /* Go get that timeout */
      timeout = i_baud_tab[baud_table_index].timeout;

    }

    baud_table_index++;

  }

  return timeout;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_baud()                                                     */
/*                                                                           */
/* Description: Gets the baud rate constant for the given number             */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used: i_baud_tab                                       */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   baud_num        unsigned long A number to convert to a baud rate        */
/*                                                                           */
/* Returns: The baud rate, or a failure indication                           */
/*                                                                           */
/*****************************************************************************/

extern speed_t seru_get_baud(unsigned long baud_num)
{

  int baud_table_index = 0;                      /* Loop counter */
  speed_t baud = SERU_GET_BAUD_FAIL; /* The baud rate constant  */


  /* Loop round the baud rate table */
  while(i_baud_tab[baud_table_index].value != 0)
  {

    /* Found a matching baud rate */
    if(baud_num == i_baud_tab[baud_table_index].value)
    {

      /* Go get that speed */
      baud = i_baud_tab[baud_table_index].speed;

    }

    baud_table_index++;

  }

  return baud;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_baud_str()                                                 */
/*                                                                           */
/* Description: Gets the baud rate string for the given baud rate            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used: i_baud_tab                                       */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type             Comments                               */
/*   ------------    ---------------  --------------------------------       */
/*   baud            speed_t          The baud rate                          */
/*   baud_str        seru_baud_str_t  The buffer for the string              */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/*****************************************************************************/

extern seru_baud_str_status_t seru_get_baud_str(speed_t baud,
  seru_baud_str_t baud_str)
{

  int baud_table_index = 0;                      /* Loop counter */
  seru_baud_str_status_t baud_str_status = SERU_GET_BAUD_STR_FAIL;
                                           /* The 'get' status */


  /* Loop round the baud rate table */
  while(i_baud_tab[baud_table_index].value != 0)
  {

    /* Found a matching baud rate */
    if(baud == i_baud_tab[baud_table_index].speed)
    {

      /* Get that baud */
      memmove(baud_str, i_baud_tab[baud_table_index].str, SERU_BAUD_STR_LEN);

      baud_str_status = SERU_GET_BAUD_STR_OK;

    }

    baud_table_index++;

  }

  return baud_str_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_flush_port()                                                   */
/*                                                                           */
/* Description: Flush the port input and output buffers                      */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type               Comments                             */
/*   ------------    ------------       -----------------------------------  */
/*   flush           seru_port_flush_t  The parameters and status for flush  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_flush_port(struct seru_port_flush_t *flush)
{

  int tcflush_return = SERU_PORT_SUCCESS; /* Return value from tcflush */
  int flush_result = SERU_FLUSH_OK;       /* Result of flush to return */


  /* Flush all input and output */
  tcflush_return =  tcflush(flush->fd, TCIOFLUSH);

  if(tcflush_return == SERU_PORT_FAILURE)
  {

    flush_result = SERU_FLUSH_FAIL;

    /* Report errno */
    flush->flush_errno = errno;

  }

  flush->status = flush_result;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_port_config()                                              */
/*                                                                           */
/* Description: Set the config of a serial port                              */
/*                                                                           */
/* Internal functions used:                                                  */
/*   i_get_port_attribs()                                                    */
/*   i_set_port_input_speed()                                                */
/*   i_set_port_output_speed()                                               */
/*   i_set_port_options()                                                    */
/*   i_set_low_latency()                                                     */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type               Comments                             */
/*   ------------    ------------       -----------------------------------  */
/*   setup           seru_port_setup_t  The setup & status for the port      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_port_config(struct seru_port_setup_t *setup)
{

  int get_attr_return = SERU_PORT_SUCCESS;
                            /* Value returned by get attributes function */
  struct termios options;   /* Structure for the port options */


  /* Clear the status */
  setup->status = SERU_STP_INIT;

  /* Clear errno  */
  setup->setup_errno = 0;

  /* Get the current options for the port */
  get_attr_return = i_get_port_attribs(setup, &options);

  /* Was get attributes successful? */
  if(get_attr_return == SERU_PORT_SUCCESS)
  {

    /* Set the input baud rate */
    i_set_port_input_speed(setup, &options);

    /* Set the output baud rate */
    i_set_port_output_speed(setup, &options);

    /* Set the port options */
    i_set_port_options(setup, &options);

    /* Set low latency, if required */
    i_set_low_latency(setup);

  }

  /* Did all go well? */
  if(setup->status == SERU_STP_INIT)
  {

    setup->status = SERU_STP_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_restore_port_state()                                           */
/*                                                                           */
/* Description: Restores the serial port settings                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used: i_saved_options                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name                Type                 Comments                       */
/*   ------------        ------------         -----------------------------  */
/*   restore_port_params seru_restore_port_t  Request params & return status */
/*                                                                           */
/* Returns: Status of restoring port, OK or fail                             */
/*                                                                           */
/*****************************************************************************/

extern int seru_restore_port_state(seru_restore_port_t *restore_port_params)
{

  int tcsetattr_return = SERU_PORT_SUCCESS;
                            /* Value returned by set attributes function */


  /* Restore the options for the port */
  tcsetattr_return = tcsetattr(restore_port_params->fd, TCSANOW,
    &i_saved_options);
  /* Warning - success returned if any set attributes were successful  */
  /* Check that attributes have been set with a tcgetattr              */

  /* Report errno, even if an error may not have occurred */
  restore_port_params->restore_errno = errno;

  return tcsetattr_return;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_save_port_state()                                              */
/*                                                                           */
/* Description: Saves the serial port settings                               */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used: i_saved_options                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name             Type              Comments                             */
/*   ------------     ------------      -----------------------------------  */
/*   save_port_params seru_save_port_t  The requested params & return status */
/*                                                                           */
/* Returns: Status indicating success or failure                             */
/*                                                                           */
/*****************************************************************************/

extern int seru_save_port_state(seru_save_port_t *save_port_params)
{

  int getattr_return = SERU_PORT_SUCCESS;
                            /* Value returned by get attributes function */


  /* Get the current options for the port */
  getattr_return = tcgetattr(save_port_params->fd, &i_saved_options);

  /* Report errno, even if an error may not have occurred */
  save_port_params->save_errno = errno;

  return getattr_return;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_lock_port()                                                    */
/*                                                                           */
/* Description: Locks a serial port                                          */
/*                                                                           */
/* Internal functions used:                                                  */
/*   i_get_lock_file_name()                                                  */
/*   i_get_lock_name()                                                       */
/*   i_check_for_lock()                                                      */
/*   i_create_lock_file()                                                    */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   serial_port      char *        The name of the serial port to lock      */
/*                                                                           */
/* Returns: The lock state - success or failure                              */
/*                                                                           */
/*****************************************************************************/

extern seru_lock_status_t seru_lock_port(char *serial_port)
{

  lock_state_t port_state = i_UNLOCKED; /* The current lock state    */
  char lock_file_name[FILENAME_MAX];    /* The lock file name & path */
  char lock_file[FILENAME_MAX];         /* The lock file name        */
  seru_lock_status_t locked_status;     /* The final lock state      */


  /* Get the name of the lock file */
  i_get_lock_file_name(serial_port, lock_file_name);

  i_get_lock_name(lock_file_name, lock_file);

  /* Is the file already locked */
  port_state = i_check_for_lock(lock_file);

  if(port_state == i_UNLOCKED)
  {

    /* Not already locked, so lock the port */
    port_state = i_create_lock_file(lock_file);

    /* Did wesuccessfullyy create the lock file? */
    if(port_state == i_UNLOCKED)
    {

      /* Failed to lock the port */ 
      locked_status = SERU_LOCK_FAIL;

    }
    else
    {

      /* Success locking the port */ 
      locked_status = SERU_LOCK_OK;

    }

  }
  else
  {

    /* The port is already locked*/ 
    locked_status = SERU_LOCKED;

  }

  return locked_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_unlock_port()                                                  */
/*                                                                           */
/* Description: Unlocks a serial port                                        */
/*                                                                           */
/* Internal functions used:                                                  */
/*   i_get_lock_file_name()                                                  */
/*   i_get_lock_name()                                                       */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   serial_port      char *        The name of the serial port to lock      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_unlock_port(char *serial_port)
{

  char lock_file_name[FILENAME_MAX];    /* The lock file name & path */
  char lock_file[FILENAME_MAX];         /* The lock file name        */


  /* Get the name of the lock file */
  i_get_lock_file_name(serial_port, lock_file_name);

  i_get_lock_name(lock_file_name, lock_file);

  /* Remove lock file, discard result, as if it fails, what can we do? */
  (void) remove(lock_file);

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_open_port()                                                    */
/*                                                                           */
/* Description: Opens serial port. Wrapper function for open()               */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type              Comments                               */
/*   ------------   ------------      -----------------------------------    */
/*   serial_port    char *            The serial port to use                 */
/*   open_params    seru_port_open_t  The requested params & return status   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_open_port(char *serial_port, seru_port_open_t *open_params)
{

  int fd;           /* File descriptor for the port */
  int fcntl_return; /* Status returned by fcntl     */
  int oldflags;     /* Flags returned by fcntl get  */


  /* Clear status */
  open_params->open_status = SERU_OPEN_INIT;

  /* Clear errno  */
  open_params->open_errno = 0;

  /* Try opening given serial port, initially non-blocking */
  fd = open(serial_port, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

  if(fd == SERU_PORT_FAILURE)
  {

    /* Could not open the port */
    open_params->open_status |= SERU_OPEN_FAIL;

    /* Report errno  */
    open_params->open_errno = errno;

  }
  else
  {

    /* Get the current flags */
    oldflags = fcntl(fd, F_GETFL, 0);

    /* Did we get the flags successfully? */
    if(oldflags != SERU_PORT_FAILURE)
    {

      /* clear O_NONBLOCK to allow read() and write() to block */
      fcntl_return = fcntl(fd, F_SETFL, oldflags & ~O_NONBLOCK);

      if(fcntl_return == SERU_PORT_FAILURE)
      {

        /* Could not open the port */
        open_params->open_status |= SERU_OPEN_FCNTL_SET_FAIL;

        /* Flag error */
        fd = SERU_PORT_FAILURE;

        /* Report errno  */
        open_params->open_errno = errno;

      }

    }
    else
    {

      /* Could not open the port */
      open_params->open_status |= SERU_OPEN_FCNTL_GET_FAIL;

      /* Flag error */
      fd = SERU_PORT_FAILURE;

      /* Report errno  */
      open_params->open_errno = errno;

    }

  }

  /* Did all go well? */
  if(open_params->open_status == SERU_OPEN_INIT)
  {

    open_params->open_status = SERU_OPEN_OK;

  }

  /* Pass back the file descriptor, or error flag */
  open_params->fd = fd;

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_close_port()                                                   */
/*                                                                           */
/* Description: Closes serial port. Wrapper function for close()             */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   close_params    seru_port_close_t  The requested params & return status */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_close_port(seru_port_close_t *close_params)
{

  int close_status; /* Status returned by close */


  /* Clear status */
  close_params->close_status = SERU_CLOSE_INIT;

  /* Clear errno  */
  close_params->close_errno = 0;

  /* Close the port */
  close_status = close(close_params->fd);

  if(close_status == SERU_PORT_FAILURE)
  {

    /* Could not close the port */
    close_params->close_status |= SERU_CLOSE_FAIL;

    /* Report errno  */
    close_params->close_errno = errno;

  }
  else
  {

    /* All went well */
    close_params->close_status |= SERU_CLOSE_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_lines()                                                    */
/*                                                                           */
/* Description: Get a serial port's control lines                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name         Type              Comments                                 */
/*   -----------  ------------      -----------------------------------      */
/*   get_lines    seru_get_lines_t  Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_get_lines(seru_get_lines_t *get_lines)
{

  int ioctl_return; /* A value passed back by a function, for some reason */


  /* Reset status */
  get_lines->status = SERU_GET_LINES_INIT;

  /* Clear errno */
  get_lines->get_errno = 0;

  /* Get the control lines */
  ioctl_return = ioctl(get_lines->fd, TIOCMGET, &(get_lines->lines) );

  /* Did things go badly? */
  if(ioctl_return == SERU_PORT_FAILURE)
  {

    /* Return failure */
    get_lines->status |= SERU_GET_LINES_FAILURE;

    get_lines->get_errno = errno;

  }
  else
  {

    get_lines->status |= SERU_GET_LINES_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_lines()                                                    */
/*                                                                           */
/* Description: Set a serial port's control lines                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name         Type              Comments                                 */
/*   -----------  ------------      -----------------------------------      */
/*   set_lines    seru_set_lines_t  Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_lines(seru_set_lines_t *set_lines)
{

  int ioctl_return; /* A value passed back by a function, for some reason */


  /* Reset status */
  set_lines->status = SERU_SET_LINES_INIT;

  /* Clear errno */
  set_lines->set_errno = 0;

  /* Set the control lines */
  ioctl_return = ioctl(set_lines->fd, TIOCMSET, &(set_lines->lines) );

  /* Did things go badly? */
  if(ioctl_return == SERU_PORT_FAILURE)
  {

    /* Return failure */
    set_lines->status |= SERU_SET_LINES_FAILURE;

    set_lines->set_errno = errno;

  }
  else
  {

    set_lines->status |= SERU_SET_LINES_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_dtr_rts()                                                  */
/*                                                                           */
/* Description: Set a serial port's DTR and RTS control lines to on or off   */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name        Type               Comments                                 */
/*   ---------   ------------       -----------------------------------      */
/*   set_dtr_rts seru_set_dtr_rts_t Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_dtr_rts(seru_set_dtr_rts_t *set_dtr_rts)
{

  seru_get_lines_t get_lines; /* Struct for getting control lines */
  seru_set_lines_t set_lines; /* Struct for setting control lines */


  /* Transfer file descriptor between structures */
  get_lines.fd = set_dtr_rts->fd;

  /* Get current control lines */
  seru_get_lines(&get_lines);

  /* Return get status */
  set_dtr_rts->get_status = get_lines.status;

  /* Did things go badly? */
  if(get_lines.status == SERU_GET_LINES_FAILURE)
  {

    /* Return get failure */
    set_dtr_rts->get_errno = get_lines.get_errno;

    /* Return set status as well */
    set_dtr_rts->set_status |= SERU_SET_LINES_FAILURE;

  }
  else
  {

    /* Is a line requested both set and cleared? */
    if( ( ((set_dtr_rts->req_lines & SERU_SET_DTR) > 0) &&
          ((set_dtr_rts->req_lines & SERU_CLEAR_DTR) > 0) ) ||
        ( ((set_dtr_rts->req_lines & SERU_SET_RTS) > 0)  &&
          ((set_dtr_rts->req_lines & SERU_CLEAR_RTS) > 0) ) )
    {

      /* Return status */
      set_dtr_rts->set_status |= SERU_SET_LINES_INVALID;

    }
    else
    {

      /* Choose lines to fiddle with */
      if( (set_dtr_rts->req_lines & SERU_SET_DTR) > 0)
      {

        get_lines.lines |= SERU_CNTRL_DTR;

      }

      if( (set_dtr_rts->req_lines & SERU_CLEAR_DTR) > 0)
      {

        get_lines.lines &= ~SERU_CNTRL_DTR;

      }

      if( (set_dtr_rts->req_lines & SERU_SET_RTS) > 0)
      {

        get_lines.lines |= SERU_CNTRL_RTS;

      }

      if( (set_dtr_rts->req_lines & SERU_CLEAR_RTS) > 0)
      {

        get_lines.lines &= ~SERU_CNTRL_RTS;

      }

      set_lines.lines = get_lines.lines;

      /* Transfer file descriptor between structures */
      set_lines.fd = set_dtr_rts->fd;

      /* Set the control lines */
      seru_set_lines(&set_lines);

      /* Return status */
      set_dtr_rts->set_status = set_lines.status;

      /* Return failure, in case things went badly */
      set_dtr_rts->set_errno = set_lines.set_errno;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_read_port_raw()                                                */
/*                                                                           */
/* Description: Receive uninterpreted byte from a serial port                */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   rx_buf         seru_rx_buf_t   Pointer to the struct to return results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Note:                                                                     */
/*  This function must be called with blocking. The read()s are assumed to   */
/*  return -1 or greater than Zero. Zero is not handled.                     */
/*                                                                           */
/*****************************************************************************/

extern void seru_read_port_raw(seru_rx_buf_t *rx_buf)
{

  ssize_t read_return = 0;   /* Value returned by a read from serial port */
  unsigned char read_buf[i_RX_BUF_SIZE];
                             /* Receive buffer                            */
  int gettimeofday_return;   /* Go on, guess                              */


  /* Reset status */
  rx_buf->rx_status = SERU_READ_INIT;

  /* Clear errno */
  rx_buf->rx_errno = 0;

  /* Clear time errno */
  rx_buf->time_errno = 0;

  /* Mark RX time failure, in case read fails */
  rx_buf->rx_time.tv_sec = SERU_TIME_FAILURE;

  rx_buf->rx_time.tv_usec = 0;

  /* Read from serial port */
  read_return = read(rx_buf->fd, read_buf, 1);

  /* Get current time */

  gettimeofday_return = gettimeofday( &(rx_buf->rx_time), NULL);

  if( (gettimeofday_return == SERU_TIME_FAILURE) || 
    (  rx_buf->rx_time.tv_sec == SERU_TIME_FAILURE) )
  {

    /* Read time failure */
    rx_buf->rx_status |= SERU_READ_TIME_FAIL;

    if (gettimeofday_return == SERU_TIME_FAILURE)
    {

      rx_buf->time_errno = errno;

    }

  }

  /* Was read successful? */
  if(read_return == SERU_PORT_FAILURE)
  {

    /* Read failure */
    rx_buf->rx_status |= SERU_READ_FAILURE;

    rx_buf->rx_errno = errno;

  }
  else
  {

    /* Have a greater than expected number of bytes been received? */
    if(read_return > 1)
    {

      rx_buf->rx_status |= SERU_READ_OVERRUN;

    }

    /* Store the byte we got */
    rx_buf->rx_byte = read_buf[0];

    /* Indicate we got a valid byte */
    rx_buf->rx_status |= SERU_READ_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_read_port()                                                    */
/*                                                                           */
/* Description: Receive byte from a serial port, interpreting errors         */
/*                                                                           */
/* Internal functions used: seru_read_port_raw()                             */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------   -----------------------------------      */
/*   rx_buf          seru_rx_buf_t  Pointer to the struct to return results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

extern void seru_read_port(seru_rx_buf_t *rx_buf)
{

  /* Read byte from serial port */
  seru_read_port_raw(rx_buf);

  /* Was read successful? */
  if( (rx_buf->rx_status & SERU_READ_OK) > 0)
  {

    /* Is byte signaled as a break, framing error or FF byte? */
    /* All else will drop through                             */
    if(rx_buf->rx_byte == (unsigned char) 0xFF)
    {

      /* Only got first byte, get more */
      seru_read_port_raw(rx_buf);

      /* Was read successful? */
      if( (rx_buf->rx_status & SERU_READ_OK) > 0)
      {

        /* Have we received a 00 denoting a break or framing error */
        if(rx_buf->rx_byte == (unsigned char) 0x00)
        {

          /* Get third byte */
          seru_read_port_raw(rx_buf);

          /* Was read successful? */
          if( (rx_buf->rx_status & SERU_READ_OK) > 0)
          {

            /* Say what's been received */
            if(rx_buf->rx_byte == (unsigned char) 0x00)
            {

              /* Could be a framing error or a break, so mark both */
              rx_buf->rx_status |= SERU_READ_FRAMERR;

              rx_buf->rx_status |= SERU_READ_BREAK;

              /* Could be a parity error, if it's enabled */
              if(i_use_parity == true)
              {

                /* Mark a parity error */
                rx_buf->rx_status |= SERU_READ_PARERR;

              }

            }
            else
            {

              /* Mark a framing error, rx byte is left in buffer */
              rx_buf->rx_status |= SERU_READ_FRAMERR;

              /* Could be a parity error, if it's enabled */
              if(i_use_parity == true)
              {

                /* Mark a parity error */
                rx_buf->rx_status |= SERU_READ_PARERR;

              }

            }

          }

        }
        else
        {

          /* Yes, it was FF received */

          /* Indicate we got a valid byte */
          rx_buf->rx_status |= SERU_READ_OK;

          /* Set RX byte back to FF, in case it got overwritten during testing */
          rx_buf->rx_byte = (unsigned char) 0xFF;

        }

      }

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_wait_for_read()                                                */
/*                                                                           */
/* Description: Wait until a read from the serial port is ready, or timeout  */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type            Comments                                */
/*   ------------    ------------    -----------------------------------     */
/*   rx_wait         seru_rx_wait_t  Ptr to struct of params and status      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_wait_for_read(seru_rx_wait_t *rx_wait)
{

  fd_set read_set;                      /* Parameters for select()    */
  struct timeval timeout;               /* Timeout parameters         */
  int select_return;                    /* Return value from select() */


  /* Reset status */
  rx_wait->rx_wait_status = SERU_RX_WAIT_INIT;

  /* Clear errno  */
  rx_wait->rx_wait_errno = 0;

  /* Clear the read fd set */
  FD_ZERO(&read_set);

  /* Put the required fd in the read fd set */
  FD_SET(rx_wait->fd, &read_set);

  /* Set the timeout, secs and microsecs */
  timeout.tv_sec = 0;

  timeout.tv_usec = rx_wait->read_timeout;

  /* Wait until we are ready to read, or timeout */
  select_return = select( (rx_wait->fd + 1), &read_set, NULL, NULL, &timeout);

  if(select_return == SERU_PORT_FAILURE)
  {

    /* Select failure */
    rx_wait->rx_wait_status |= SERU_RX_WAIT_FAILURE;

    /* Report errno */
    rx_wait->rx_wait_errno = errno;

  }
  else
  {

    /* Got something to read? */
    if(select_return > 0)
    {

      rx_wait->rx_wait_status |= SERU_RX_WAIT_READY;

    }
    else /* or did we timeout */
    {

      rx_wait->rx_wait_status |= SERU_RX_WAIT_TIMEOUT;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_write_port()                                                   */
/*                                                                           */
/* Description: Write byte to serial port                                    */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type            Comments                                */
/*   ------------    ------------   -----------------------------------      */
/*   tx_buf          seru_tx_buf_t  Pointer to the struct of byte to send    */
/*                                  and to return results                    */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_write_port(seru_tx_buf_t *tx_buf)
{

  ssize_t write_return;     /* Value returned by a write to serial port */
  int gettimeofday_return;  /* The value returning after calling the    */
                            /* gettimeofday function                    */


  /* Reset status */
  tx_buf->tx_status = SERU_WRITE_INIT;

  /* Clear errno  */
  tx_buf->tx_errno = 0;

  /* Clear time errno */
  tx_buf->time_errno = 0;

  /* Mark RX time failure, in case read fails */
  tx_buf->tx_time.tv_sec = SERU_TIME_FAILURE;

  tx_buf->tx_time.tv_usec = 0;

  /* Write byte to serial port */
  write_return = write(tx_buf->fd, &(tx_buf->tx_byte), 1);

  /* Get current time */
  gettimeofday_return = gettimeofday( &(tx_buf->tx_time), NULL);

  if( (gettimeofday_return == SERU_TIME_FAILURE) || 
    (  tx_buf->tx_time.tv_sec == SERU_TIME_FAILURE) )
  {

    /* Read time failure */
    tx_buf->tx_status |= SERU_WRITE_TIME_FAIL;

    if (gettimeofday_return == SERU_TIME_FAILURE)
    {

      tx_buf->time_errno = errno;

    }

  }

  /* Was write successful? */
  if(write_return == SERU_PORT_FAILURE)
  {

    /* Read failure */
    tx_buf->tx_status |= SERU_WRITE_FAILURE;

    tx_buf->tx_errno = errno;

  }
  else
  {

    /* Indicate we sent without problem */
    tx_buf->tx_status |= SERU_WRITE_OK;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: seru_wait_for_write()                                               */
/*                                                                           */
/* Description: Wait until the serial port is ready to send a byte           */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type            Comments                                */
/*   ------------    ------------    -----------------------------------     */
/*   tx_wait         seru_tx_wait_t  Ptr to struct of params and status      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void seru_wait_for_write(seru_tx_wait_t *tx_wait)
{

  fd_set write_set;                     /* Parameters for select()    */
  struct timeval timeout;               /* Timeout parameters         */
  int select_return;                    /* Return value from select() */


  /* Reset status */
  tx_wait->tx_wait_status = SERU_TX_WAIT_INIT;

  /* Clear errno  */
  tx_wait->tx_wait_errno = 0;

  /* Clear the write fd set */
  FD_ZERO(&write_set);

  /* Put the required fd in the write fd set */
  FD_SET(tx_wait->fd, &write_set);

  /* Set the timeout, secs and microsecs */
  timeout.tv_sec = 0;

  timeout.tv_usec = tx_wait->write_timeout;

  /* Wait until we are ready to write, or timeout */
  select_return = select( (tx_wait->fd + 1), NULL, &write_set, NULL, &timeout);

  if(select_return == SERU_PORT_FAILURE)
  {

    /* Select failure */
    tx_wait->tx_wait_status |= SERU_TX_WAIT_FAILURE;

    /* Report errno */
    tx_wait->tx_wait_errno = errno;

  }
  else
  {

    /* Ready to write? */
    if(select_return > 0)
    {

      tx_wait->tx_wait_status |= SERU_TX_WAIT_READY;

    }
    else /* or did we timeout */
    {

      tx_wait->tx_wait_status |= SERU_TX_WAIT_TIMEOUT;

    }

  }

}
