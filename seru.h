/*****************************************************************************/
/*                                                                           */
/* Module: seru.h                                                            */
/*                                                                           */
/* Description: Header file for seru.c                                       */
/*                                                                           */
/* Copyright (C) 2004, 2010, 2011 David Wilson Clarke                        */
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

#ifndef SERU_H

#define SERU_H

/*****************************************************************************/
/*      INCLUDED FILES (dependencies)                                        */
/*****************************************************************************/

#include <termios.h>     /* Standard serial I/O definitions                */
#include <limits.h>      /* Variable max sizes - ULONG_MAX                 */
#include <sys/time.h>    /* Standard time definitions - timeval            */
#include <sys/ioctl.h>   /* ioctl() stuff                                  */
#include <stdbool.h>     /* Boolean types                                  */

/*****************************************************************************/
/*      MACRO DEFINITIONS                                                    */
/*****************************************************************************/

/*****************************************************************************/
/*  Name:                                                                    */
/*  Description:                                                             */
/*  Parameters:                                                              */
/*****************************************************************************/


/*****************************************************************************/
/*      TYPE DEFINITIONS                                                     */
/*****************************************************************************/

/* Enums & constants */

#define SERU_GET_TIMEOUT_FAIL ULONG_MAX
                                     /* Failure to get a timeout indicator   */

#define SERU_GET_BAUD_FAIL UINT_MAX  /* Failure to get a baud rate indicator */

enum { SERU_PORT_FAILURE = -1 };     /* Serial port access failure indicator */

enum { SERU_PORT_SUCCESS = 0 };      /* Serial port access success indicator */

enum { SERU_TIME_FAILURE = -1 };     /* Reading time failure indicator       */

enum { SERU_BAUD_STR_LEN = 10 };     /* Length of the baud rate string       */

enum { SERU_TIME_MAX = LONG_MAX };   /* Maximum value of a time_t            */

enum { SERU_TIMEOUT_MAX = 9999999 }; /* Maximum receive timeout in usecs     */

enum { SERU_CNTRL_LE  = TIOCM_LE  }; /* Control line - DSR/Line Enable       */

enum { SERU_CNTRL_DTR = TIOCM_DTR }; /* Control line - DTR                   */

enum { SERU_CNTRL_RTS = TIOCM_RTS }; /* Control line - RTS                   */

enum { SERU_CNTRL_ST  = TIOCM_ST };  /* Control line - Secondary TXD         */

enum { SERU_CNTRL_SR  = TIOCM_SR };  /* Control line - Secondary RXD         */

enum { SERU_CNTRL_CTS = TIOCM_CTS }; /* Control line - CTS                   */

enum { SERU_CNTRL_CD  = TIOCM_CD };  /* Control line - CD                    */

enum { SERU_CNTRL_RI  = TIOCM_RI };  /* Control line - RI                    */

enum { SERU_CNTRL_DSR = TIOCM_DSR }; /* Control line - DSR                   */

/* Types */

/* Type for port lock status */
typedef enum seru_lock_status_t
{SERU_LOCKED, SERU_LOCK_FAIL, SERU_LOCK_OK} seru_lock_status_t;

/* Type for comms timeouts                */
typedef unsigned long seru_timeout_t;

/* Type for baud rate as a string         */
typedef unsigned char seru_baud_str_t[SERU_BAUD_STR_LEN];

/* Type for baud rate string 'get' status */
typedef enum seru_baud_str_status_t
  {SERU_GET_BAUD_STR_FAIL, SERU_GET_BAUD_STR_OK} seru_baud_str_status_t;

/* Type for serial number of data bits */
typedef enum seru_data_bits_t
{
  SERU_5_DATA_BITS = 5,
  SERU_6_DATA_BITS = 6,
  SERU_7_DATA_BITS = 7,
  SERU_8_DATA_BITS = 8
} seru_data_bits_t;

/* Type for serial parity */
typedef enum seru_parity_t
{
  SERU_ODD_PARITY = 1,
  SERU_EVEN_PARITY,
  SERU_NO_PARITY
} seru_parity_t;

/* Type for serial number of stop bits */
typedef enum seru_stop_bits_t
{
  SERU_1_STOP_BIT = 1,
  SERU_2_STOP_BIT = 2
} seru_stop_bits_t;

/* Type for serial port flush status */
typedef enum seru_flush_status_t
{
  SERU_FLUSH_INIT        = 0x0000,  /* Initialised status                 */
  SERU_FLUSH_OK          = 0x0001,  /* Flush worked                       */
  SERU_FLUSH_FAIL        = 0x0002   /* Unable to flush I/O buffers        */
} seru_flush_status_t;

/* Buffer type for flushing serial ports */
typedef struct seru_port_flush_t
{
  int fd;                     /* The port file descriptor               */
  seru_flush_status_t status; /* The flush status, any errors, or OK    */
  int flush_errno;            /* errno on flush fail                    */
} seru_port_flush_t;

/* Type for serial port setup status */
/* These are bitmapped flags         */
typedef enum seru_setup_status_t
{
  SERU_STP_INIT           = 0x0000,  /* Initialised status                 */
  SERU_STP_OK             = 0x0001,  /* Setup worked                       */
  SERU_STP_GET_OPTS_FAIL  = 0x0002,  /* Unable to get options during setup */
  SERU_STP_SET_OPTS_FAIL  = 0x0004,  /* Unable to set options during setup */
  SERU_STP_BAD_DATA_BITS  = 0x0008,  /* Invalid data bits given            */
  SERU_STP_BAD_PARITY     = 0x0010,  /* Invalid parity given               */
  SERU_STP_BAD_STOP_BITS  = 0x0020,  /* Invalid stop bits given            */
  SERU_STP_IBAUD_FAIL     = 0x0040,  /* Unable to set input baud rate      */
  SERU_STP_OBAUD_FAIL     = 0x0080,   /* Unable to set output baud rate    */
  SERU_STP_IOCTL_GET_FAIL = 0x0100,  /* Ioctl get failed during setup      */
  SERU_STP_IOCTL_SET_FAIL = 0x0200   /* Ioctl set failed during setup      */
} seru_setup_status_t;

/* Buffer type for setting up serial ports */
typedef struct seru_port_setup_t
{
  int fd;                     /* The port file descriptor               */
  speed_t speed;              /* The port baud rate                     */
  seru_data_bits_t data_bits; /* The number of data bits                */
  seru_parity_t parity;       /* The parity                             */
  seru_stop_bits_t stop_bits; /* The number of stop bits                */
  bool low_latency;           /* Request low latency                    */
  seru_setup_status_t status; /* The setup status, any errors, or OK    */
  int setup_errno;            /* errno on setup fail                    */
} seru_port_setup_t;

/* Type for restoring serial port's state*/
typedef struct seru_restore_port_t
{
  int fd;                         /* The port file descriptor   */
  int restore_errno;              /* errno on save fail         */
} seru_restore_port_t;

/* Type for saving serial port's state */
typedef struct seru_save_port_t
{
  int fd;                         /* The port file descriptor   */
  int save_errno;                 /* errno on save fail         */
} seru_save_port_t;

/* Type for serial port open status */
/* These are bitmapped flags        */
typedef enum seru_open_status_t
{
  SERU_OPEN_INIT           = 0x0000,  /* Initialised status           */
  SERU_OPEN_OK             = 0x0001,  /* Open worked                  */
  SERU_OPEN_FAIL           = 0x0002,  /* Open failed                  */
  SERU_OPEN_FCNTL_GET_FAIL = 0x0004,  /* Fcntl get failed during open */
  SERU_OPEN_FCNTL_SET_FAIL = 0x0008   /* Fcntl set failed during open */
} seru_open_status_t;

/* Type for opening serial port */
typedef struct seru_port_open_t
{
  int fd;                         /* The port file descriptor   */
  seru_open_status_t open_status; /* Status of open, OK or fail */
  int open_errno;                 /* errno on open fail         */
} seru_port_open_t;

/* Type for serial port close status */
/* These are bitmapped flags         */
typedef enum seru_close_status_t
{
  SERU_CLOSE_INIT      = 0x0000, /* Initialised status        */
  SERU_CLOSE_OK        = 0x0001,  /* Close worked             */
  SERU_CLOSE_FAIL      = 0x0002   /* Close failed             */
} seru_close_status_t;

/* Type for closing serial ports */
typedef struct seru_port_close_t
{
  int fd;                           /* The port file descriptor    */
  seru_close_status_t close_status; /* Status of close, OK or fail */
  int close_errno;                  /* errno on close fail         */
} seru_port_close_t;

/* Type for getting control lines status */
/* These are bitmapped flags             */
typedef enum seru_get_line_status_t
{
  SERU_GET_LINES_INIT      = 0x0000, /* Initialised status          */
  SERU_GET_LINES_FAILURE   = 0x0001, /* Unable to get control lines */
  SERU_GET_LINES_OK        = 0x0002  /* Get was successful          */
} seru_get_line_status_t;

/* Buffer type for getting the serial control lines */
typedef struct seru_get_lines_t
{
  int fd;                        /* The port file descriptor              */
  int lines;                     /* The returned control line data        */
  seru_get_line_status_t status; /* The get status, any errors, or OK     */
  int get_errno;                 /* errno on get fail                     */
} seru_get_lines_t;

/* Type for setting control lines status */
/* These are bitmapped flags             */
typedef enum seru_set_line_status_t
{
  SERU_SET_LINES_INIT      = 0x0000, /* Initialised status          */
  SERU_SET_LINES_FAILURE   = 0x0001, /* Unable to set control lines */
  SERU_SET_LINES_OK        = 0x0002, /* Set was successful          */
  SERU_SET_LINES_INVALID   = 0x0004  /* Requested invalid lines     */
} seru_set_line_status_t;

/* Buffer type for setting the serial control lines */
typedef struct seru_set_lines_t
{
  int fd;                        /* The port file descriptor              */
  int lines;                     /* The required control line data        */
  seru_set_line_status_t status; /* The set status, any errors, or OK     */
  int set_errno;                 /* errno on set fail                     */
} seru_set_lines_t;

/* Type for requesting drt or rts to be set or cleared */
/* These are bitmapped flags         */
typedef enum seru_req_lines_t
{
  SERU_SET_DTR        = 0x0001,  /* Request DTR set          */
  SERU_CLEAR_DTR      = 0x0002,  /* Request DTR cleared      */
  SERU_SET_RTS        = 0x0004,  /* Request RTS set          */
  SERU_CLEAR_RTS      = 0x0008   /* Request RTS cleared      */
} seru_req_lines_t;

/* Buffer type for setting the dtr amd rts control lines */
typedef struct seru_set_dtr_rts_t
{
  int fd;                            /* The port file descriptor            */
  int req_lines;                     /* The returned control line data      */
  seru_get_line_status_t get_status; /* The get status, any errors, or OK   */
  int get_errno;                     /* errno on get fail                   */
  seru_set_line_status_t set_status; /* The set status, any errors, or OK   */
  int set_errno;                     /* errno on set fail                   */
} seru_set_dtr_rts_t;

/* Type for serial receive status */
/* These are bitmapped flags      */
typedef enum seru_rx_status_t
{
  SERU_READ_INIT      = 0x0000, /* Initialised status              */
  SERU_READ_FAILURE   = 0x0001, /* Unable to read serial port      */
  SERU_READ_OVERRUN   = 0x0002, /* More than one byte was received */
  SERU_READ_FRAMERR   = 0x0004, /* Framing error                   */
  SERU_READ_PARERR    = 0x0008, /* Parity received                 */
  SERU_READ_BREAK     = 0x0010, /* Break received                  */
  SERU_READ_TIME_FAIL = 0x0020, /* Unable to read time             */
  SERU_READ_OK        = 0x0040  /* Read was successful             */
} seru_rx_status_t;

/* Buffer type for the receipt of a serial byte */
typedef struct seru_rx_buf_t
{
  int fd;                     /* The port file descriptor              */
  unsigned char rx_byte;      /* The received byte                     */
  seru_rx_status_t rx_status; /* The receive status, any errors, or OK */
  int rx_errno;               /* errno on rx fail                      */
  struct timeval rx_time;     /* The time the byte was received        */
  int time_errno;             /* errno on read time fail               */
} seru_rx_buf_t;

/* Type for wait for serial RX status */
/* These are bitmapped flags          */
typedef enum seru_rx_wait_status_t
{
  SERU_RX_WAIT_INIT    = 0x0000, /* Initialised status                     */
  SERU_RX_WAIT_FAILURE = 0x0001, /* Unable to wait for read on serial port */
  SERU_RX_WAIT_TIMEOUT = 0x0002, /* Timeout on read                        */
  SERU_RX_WAIT_READY   = 0x0004  /* Byte ready for read                    */
} seru_rx_wait_status_t;

/* Type for wait for RX */
typedef struct seru_rx_wait_t
{
  int fd;                               /* The port file descriptor      */
  seru_timeout_t read_timeout;          /* The wait timeout              */
  seru_rx_wait_status_t rx_wait_status; /* The receive wait status       */
  int rx_wait_errno;                    /* errno on rx wait fail         */
} seru_rx_wait_t;

/* Type for serial transmit status */
/* These are bitmapped flags       */
typedef enum seru_tx_status_t
{
  SERU_WRITE_INIT      = 0x0000, /* Initialised status             */
  SERU_WRITE_FAILURE   = 0x0001, /* Unable to write to serial port */
  SERU_WRITE_OK        = 0x0002, /* Write was successful           */
  SERU_WRITE_TIME_FAIL = 0x0004  /* Unable to read time            */
} seru_tx_status_t;

/* Buffer type for the transmision of a serial byte */
typedef struct seru_tx_buf_t
{

  int fd;                     /* The port file descriptor               */
  unsigned char tx_byte;      /* The byte to transmit                   */
  seru_tx_status_t tx_status; /* The transmit status, any errors, or OK */
  int tx_errno;               /* errno on tx fail                       */
  struct timeval tx_time;     /* The time the byte was transmitted      */
  int time_errno;             /* errno on read time fail                */
} seru_tx_buf_t;

/* Type for wait for serial TX status */
/* These are bitmapped flags          */
typedef enum seru_tx_wait_status_t
{
  SERU_TX_WAIT_INIT    = 0x0000, /* Initialised status                      */
  SERU_TX_WAIT_FAILURE = 0x0001, /* Unable to wait for write on serial port */
  SERU_TX_WAIT_TIMEOUT = 0x0002, /* Timeout on write                        */
  SERU_TX_WAIT_READY   = 0x0004  /* Port ready for write                    */
} seru_tx_wait_status_t;

/* Type for wait for TX */
typedef struct seru_tx_wait_t
{
  int fd;                               /* The port file descriptor      */
  seru_timeout_t write_timeout;         /* The wait timeout              */
  seru_tx_wait_status_t tx_wait_status; /* The transmit wait status      */
  int tx_wait_errno;                    /* errno on tx wait fail         */
} seru_tx_wait_t;


/*****************************************************************************/
/*      GLOBAL VARIABLES                                                     */
/*****************************************************************************/


/*****************************************************************************/
/*      FUNCTION PROTOTYPES                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_timeout()                                                  */
/*                                                                           */
/* Description: Gets the timeout for the given baud rate                     */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   baud            speed_t       The baud rate                             */
/*                                                                           */
/* Returns: The timeout, or a failure indication                             */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions:                                                          */
/*                                                                           */
/*****************************************************************************/

extern seru_timeout_t seru_get_timeout(speed_t baud);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_baud()                                                     */
/*                                                                           */
/* Description: Gets the baud rate constant for the given number             */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   baud_num        unsigned long A number to convert to a baud rate        */
/*                                                                           */
/* Returns: The baud rate, or a failure indication                           */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions:                                                          */
/*                                                                           */
/*****************************************************************************/

extern speed_t seru_get_baud(unsigned long baud_num);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_baud_str()                                                 */
/*                                                                           */
/* Description: Gets the baud rate string for the given baud rate            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type             Comments                               */
/*   ------------    ---------------  --------------------------------       */
/*   baud            speed_t          The baud rate                          */
/*   baud_str        seru_baud_str_t  The buffer for the string              */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions: String returned in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern seru_baud_str_status_t seru_get_baud_str(speed_t baud,
  seru_baud_str_t baud_str);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_flush_port()                                                   */
/*                                                                           */
/* Description: Flush the port input and output buffers                      */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type               Comments                             */
/*   ------------    ------------       -----------------------------------  */
/*   flush           seru_port_flush_t  The parameters and status for flush  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: The port buffers will be flushed.                        */
/*                                                                           */
/*****************************************************************************/

extern void seru_flush_port(struct seru_port_flush_t *flush);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_port_config()                                              */
/*                                                                           */
/* Description: Sets the config of a serial port                             */
/*                                                                           */
/* Parameters                                                                */
/*   Name            Type               Comments                             */
/*   ------------    ------------       -----------------------------------  */
/*   setup           seru_port_setup_t  The setup & status for the port      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port configured, unless error occured                    */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_port_config(struct seru_port_setup_t *setup);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_restore_port_state()                                           */
/*                                                                           */
/* Description: Restores the serial port settings                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name                Type                 Comments                       */
/*   ------------        ------------         -----------------------------  */
/*   restore_port_params seru_restore_port_t  Request params & return status */
/*                                                                           */
/* Returns: Status of restoring port, OK or fail                             */
/*                                                                           */
/* Pre-conditions: Port open & port state saved                              */
/*                                                                           */
/* Post-conditions: Port state restored                                      */
/*                                                                           */
/*****************************************************************************/

extern int seru_restore_port_state(seru_restore_port_t *restore_port_params);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_save_port_state()                                              */
/*                                                                           */
/* Description: Saves the serial port settings                               */
/*                                                                           */
/* Parameters:                                                               */
/*   Name             Type              Comments                             */
/*   ------------     ------------      -----------------------------------  */
/*   save_port_params seru_save_port_t  The requested params & return status */
/*                                                                           */
/* Returns:Status indicating success or failure                              */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port state saved                                         */
/*                                                                           */
/*****************************************************************************/

extern int seru_save_port_state(seru_save_port_t *save_port_params);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_lock_port()                                                    */
/*                                                                           */
/* Description: Locks a serial port                                          */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type          Comments                                  */
/*   ------------    ------------  -----------------------------------       */
/*   serial_port     char *        The name of the serial port to lock       */
/*                                                                           */
/* Returns: The lock state - success or failure                              */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions: Lock file created, if possible.                          */
/*                                                                           */
/*****************************************************************************/

extern seru_lock_status_t seru_lock_port(char *serial_port);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_unlock_port()                                                  */
/*                                                                           */
/* Description: Unlocks a serial port                                        */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type          Comments                                  */
/*   ------------    ------------  -----------------------------------       */
/*   serial_port     char *        The name of the serial port to lock       */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Lock file exists                                          */
/*                                                                           */
/* Post-conditions: Lock file deleted, if possible.                          */
/*                                                                           */
/*****************************************************************************/

extern void seru_unlock_port(char *serial_port);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_open_port()                                                    */
/*                                                                           */
/* Description: Opens serial port. Wrapper function for open()               */
/*                                                                           */
/* Parameters                                                                */
/*   Name           Type              Comments                               */
/*   ------------   ------------      -----------------------------------    */
/*   serial_port    char *            The serial port to use                 */
/*   open_params    seru_port_open_t  The requested params & return status   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port closed                                               */
/*                                                                           */
/* Post-conditions: Port open, unless error occured                          */
/*                                                                           */
/*****************************************************************************/

extern void seru_open_port(char *serial_port, seru_port_open_t *open_params);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_close_port()                                                   */
/*                                                                           */
/* Description: Closes serial port. Wrapper function for close()             */
/*                                                                           */
/* Parameters                                                                */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   close_params    seru_port_close_t  The requested params & return status */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port closed                                              */
/*                                                                           */
/*****************************************************************************/

extern void seru_close_port(seru_port_close_t *close_params);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_get_lines()                                                    */
/*                                                                           */
/* Description: Get a serial port's control lines                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name         Type              Comments                                 */
/*   -----------  ------------      -----------------------------------      */
/*   get_lines    seru_get_lines_t  Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Returned control lines status in buffer                  */
/*                                                                           */
/*****************************************************************************/

extern void seru_get_lines(seru_get_lines_t *get_lines);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_lines()                                                    */
/*                                                                           */
/* Description: Set a serial port's control lines                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name         Type              Comments                                 */
/*   -----------  ------------      -----------------------------------      */
/*   set_lines    seru_set_lines_t  Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Control lines altered & returned status in buffer        */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_lines(seru_set_lines_t *set_lines);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_set_dtr_rts()                                                  */
/*                                                                           */
/* Description: Set a serial port's DTR and RTS control lines to on or off   */
/*                                                                           */
/* Parameters:                                                               */
/*   Name        Type               Comments                                 */
/*   ---------   ------------       -----------------------------------      */
/*   set_dtr_rts seru_set_dtr_rts_t Ptr to struct with params & for results  */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Control lines altered & returned status in buffer        */
/*                                                                           */
/*****************************************************************************/

extern void seru_set_dtr_rts(seru_set_dtr_rts_t *set_dtr_rts);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_read_port_raw()                                                */
/*                                                                           */
/* Description: Receive uninterpreted bytes on a serial port                 */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   rx_buf          seru_rx_buf   Pointer to the struct to return results   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned byte and status in buffer                       */
/*                                                                           */
/*****************************************************************************/

extern void seru_read_port_raw(seru_rx_buf_t *rx_buf);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_read_port()                                                    */
/*                                                                           */
/* Description: Receive byte from a serial port, interpreting errors         */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   rx_buf          seru_rx_buf   Pointer to the struct to return results   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned byte and status in buffer                       */
/*                                                                           */
/*****************************************************************************/

extern void seru_read_port(seru_rx_buf_t *rx_buf);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_wait_for_read()                                                */
/*                                                                           */
/* Description: Wait until a read from the serial port is ready, or timeout  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type            Comments                                */
/*   ------------    ------------    -----------------------------------     */
/*   rx_wait         seru_rx_wait_t  Ptr to struct of params and status      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern void seru_wait_for_read(seru_rx_wait_t *rx_wait);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_write_port()                                                   */
/*                                                                           */
/* Description: Write byte to serial port                                    */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   tx_buf          seru_tx_buf   Pointer to the struct of byte to send and */
/*                                 to return results                         */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern void seru_write_port(seru_tx_buf_t *tx_buf);


/*****************************************************************************/
/*                                                                           */
/* Name: seru_wait_for_write()                                               */
/*                                                                           */
/* Description: Wait until the serial port is ready to send a byte           */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type            Comments                                */
/*   ------------    ------------    -----------------------------------     */
/*   tx_wait         seru_tx_wait_t  Ptr to struct of params and status      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern void seru_wait_for_write(seru_tx_wait_t *tx_wait);


#endif /* SERU_H */

