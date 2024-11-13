/*****************************************************************************/
/*                                                                           */
/* Module: serp.h                                                            */
/*                                                                           */
/* Description: Header file for serp.c                                       */
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

#ifndef SERP_H

#define SERP_H

/*****************************************************************************/
/*      INCLUDED FILES (dependencies)                                        */
/*****************************************************************************/

#include <stdbool.h>     /* Boolean types                                   */
#include <sys/time.h>    /* Standard time definitions - timeval             */
#include <stdbool.h>     /* Boolean types                                  */
#include "seru.h"        /* Serial utilities library                        */

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

#define SERP_GET_TIMEOUT_FAIL SERU_GET_TIMEOUT_FAIL
                                     /* Failure to get a timeout indicator   */

#define SERP_GET_BAUD_FAIL SERU_GET_BAUD_FAIL
                                     /* Failure to get a baud rate indicator */

enum { SERP_PORT_FAILURE = SERU_PORT_FAILURE };
                                     /* Serial port access failure indicator */

enum { SERP_PORT_SUCCESS = SERU_PORT_SUCCESS };
                                     /* Serial port access success indicator */

enum { SERP_TIME_FAILURE = SERU_TIME_FAILURE };
                                     /* Reading time failure indicator       */

enum { SERP_TIMEOUT_MAX = SERU_TIMEOUT_MAX };
                                     /* Maximum receive timeout in microsecs */

enum { SERP_CNTRL_LE  = SERU_CNTRL_LE  }; /* Control line - DSR/Line Enable  */

enum { SERP_CNTRL_DTR = SERU_CNTRL_DTR }; /* Control line - DTR              */

enum { SERP_CNTRL_RTS = SERU_CNTRL_RTS }; /* Control line - RTS              */

enum { SERP_CNTRL_ST  = SERU_CNTRL_ST };  /* Control line - Secondary TXD    */

enum { SERP_CNTRL_SR  = SERU_CNTRL_SR };  /* Control line - Secondary RXD    */

enum { SERP_CNTRL_CTS = SERU_CNTRL_CTS }; /* Control line - CTS              */

enum { SERP_CNTRL_CD  = SERU_CNTRL_CD };  /* Control line - CD               */

enum { SERP_CNTRL_RI  = SERU_CNTRL_RI };  /* Control line - RI               */

enum { SERP_CNTRL_DSR = SERU_CNTRL_DSR }; /* Control line - DSR              */

/* Types */

/* Type for port lock status */
typedef enum serp_lock_status_t
{SERP_LOCK_FAIL, SERP_LOCK_OK} serp_lock_status_t;

/* Type for comms timeouts                */
typedef seru_timeout_t serp_timeout_t;

/* Type for baud rate string 'get' status */
typedef enum serp_baud_str_status_t
{
  SERP_GET_BAUD_STR_FAIL = SERU_GET_BAUD_STR_FAIL,
  SERP_GET_BAUD_STR_OK = SERU_GET_BAUD_STR_OK
} serp_baud_str_status_t;

/* Type for baud rate as a string         */
typedef seru_baud_str_t serp_baud_str_t;

/* Type for serial number of data bits */
typedef enum serp_data_bits_t
{
  SERP_5_DATA_BITS = SERU_5_DATA_BITS,
  SERP_6_DATA_BITS = SERU_6_DATA_BITS,
  SERP_7_DATA_BITS = SERU_7_DATA_BITS,
  SERP_8_DATA_BITS = SERU_8_DATA_BITS
} serp_data_bits_t;

/* Type for serial parity */
typedef enum serp_parity_t
{
  SERP_ODD_PARITY  = SERU_ODD_PARITY,
  SERP_EVEN_PARITY = SERU_EVEN_PARITY,
  SERP_NO_PARITY   = SERU_NO_PARITY
} serp_parity_t;

/* Type for serial number of stop bits */
typedef enum serp_stop_bits_t
{
  SERP_1_STOP_BIT = SERU_1_STOP_BIT,
  SERP_2_STOP_BIT = SERU_2_STOP_BIT
} serp_stop_bits_t;

/* Buffer type for setting up serial ports */
typedef struct serp_port_setup_t
{
  int fd;                     /* The port file descriptor               */
  speed_t speed;              /* The port baud rate                     */
  serp_data_bits_t data_bits; /* The number of data bits                */
  serp_parity_t parity;       /* The parity                             */
  serp_stop_bits_t stop_bits; /* The number of stop bits                */
  bool low_latency;           /* Request low latency                    */
} serp_port_setup_t;

/* Type for getting control lines status */
/* These are bitmapped flags             */
typedef enum serp_get_line_status_t
{
  SERP_GET_LINES_FAILURE  = SERU_GET_LINES_FAILURE, 
                                                   /* Can't to get ctl lines */
  SERP_GET_LINES_OK       = SERU_GET_LINES_OK      /* Get was successful     */
} serp_get_line_status_t;

/* Buffer type for getting the serial control lines */
typedef struct serp_get_lines_t
{
  int fd;                        /* The port file descriptor              */
  int lines;                     /* The returned control line data        */
  serp_get_line_status_t status; /* The get status, any errors, or OK     */
} serp_get_lines_t;

/* Type for setting control lines status */
/* These are bitmapped flags             */
typedef enum serp_set_line_status_t
{
  SERP_SET_LINES_FAILURE  = SERU_SET_LINES_FAILURE, 
                                                  /* Can't to set ctl lines */
  SERP_SET_LINES_OK       = SERU_SET_LINES_OK     /* Set was successful     */
} serp_set_line_status_t;

/* Buffer type for setting the serial control lines */
typedef struct serp_set_lines_t
{
  int fd;                        /* The port file descriptor              */
  int lines;                     /* The required control line data        */
  serp_set_line_status_t status; /* The set status, any errors, or OK     */
} serp_set_lines_t;

/* Type for requesting drt or rts to be set or cleared */
/* These are bitmapped flags         */
typedef enum serp_req_lines_t
{
  SERP_SET_DTR   = SERU_SET_DTR,    /* Request DTR set          */
  SERP_CLEAR_DTR = SERU_CLEAR_DTR,  /* Request DTR cleared      */
  SERP_SET_RTS   = SERU_SET_RTS,    /* Request RTS set          */
  SERP_CLEAR_RTS = SERU_CLEAR_RTS   /* Request RTS cleared      */
} serp_req_lines_t;

/* Buffer type for setting the dtr amd rts control lines */
typedef struct serp_set_dtr_rts_t
{
  int fd;                            /* The port file descriptor            */
  serp_req_lines_t req_lines;        /* The requested control line state    */
  serp_get_line_status_t get_status; /* The get status, any errors, or OK   */
  int get_errno;                     /* errno on get fail                   */
  serp_set_line_status_t set_status; /* The set status, any errors, or OK   */
  int set_errno;                     /* errno on set fail                   */
} serp_set_dtr_rts_t;

/* Type for serial receive status */
/* These are bitmapped flags      */
typedef enum serp_rx_status_t
{
  SERP_READ_FAILURE   = SERU_READ_FAILURE,   /* Unable to read serial port   */
  SERP_READ_OVERRUN   = SERU_READ_OVERRUN,   /* More than one byte was rx'ed */
  SERP_READ_FRAMERR   = SERU_READ_FRAMERR,   /* Framing error                */
  SERP_READ_PARERR    = SERU_READ_PARERR,    /* Parity error                 */
  SERP_READ_BREAK     = SERU_READ_BREAK,     /* Break received               */
  SERP_READ_TIME_FAIL = SERU_READ_TIME_FAIL, /* Unable to read time          */
  SERP_READ_OK        = SERU_READ_OK         /* Read was successful          */
} serp_rx_status_t;

/* Buffer type for the receipt of a serial byte */
typedef struct serp_rx_buf_t
{
  int fd;                     /* The port file descriptor              */
  unsigned char rx_byte;      /* The received byte                     */
  serp_rx_status_t rx_status; /* The receive status, any errors, or OK */
  struct timeval rx_time;     /* The time the byte was received        */
} serp_rx_buf_t;

/* Type for wait for serial RX status */
typedef enum serp_rx_wait_status_t
{
  SERP_RX_WAIT_FAILURE = SERU_RX_WAIT_FAILURE, /* Wait fail            */
  SERP_RX_WAIT_TIMEOUT = SERU_RX_WAIT_TIMEOUT, /* Timeout on read      */
  SERP_RX_WAIT_READY   = SERU_RX_WAIT_READY    /* Byte ready for read  */
} serp_rx_wait_status_t;

/* Type for serial transmit status */
/* These are bitmapped flags       */
typedef enum serp_tx_status_t
{
  SERP_WRITE_INIT      = SERU_WRITE_INIT,     /* Initialised status      */
  SERP_WRITE_FAILURE   = SERU_WRITE_FAILURE,  /* Unable to write to port */
  SERP_WRITE_OK        = SERU_WRITE_OK,       /* Write was successful    */
  SERP_WRITE_TIME_FAIL = SERU_WRITE_TIME_FAIL /* Write was successful    */
} serp_tx_status_t;

/* Buffer type for the transmision of a serial byte */
typedef struct serp_tx_buf_t
{
  int fd;                     /* The port file descriptor               */
  unsigned char tx_byte;      /* The byte to transmit                   */
  serp_tx_status_t tx_status; /* The transmit status, any errors, or OK */
  struct timeval tx_time;     /* The time the byte was transmitted      */
  int time_errno;             /* errno on read time fail                */
} serp_tx_buf_t;

/* Type for wait for serial TX status */
/* These are bitmapped flags          */
typedef enum serp_tx_wait_status_t
{
  SERP_TX_WAIT_FAILURE = SERU_TX_WAIT_FAILURE, /* Unable to wait for write */
  SERP_TX_WAIT_TIMEOUT = SERU_TX_WAIT_TIMEOUT, /* Timeout on write         */
  SERP_TX_WAIT_READY   = SERU_TX_WAIT_READY    /* Port ready for write     */
} serp_tx_wait_status_t;


/*****************************************************************************/
/*      GLOBAL VARIABLES                                                     */
/*****************************************************************************/


/*****************************************************************************/
/*      FUNCTION PROTOTYPES                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_timeout()                                                  */
/*                                                                           */
/* Description: Gets the timeout for the current baud rate                   */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        ----------------------------------   */
/*   baud           speed_t             The baud rate                        */
/*                                                                           */
/* Returns: The timeout, or a failure indication                             */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions:                                                          */
/*                                                                           */
/*****************************************************************************/

extern serp_timeout_t serp_get_timeout(speed_t baud);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_baud()                                                     */
/*                                                                           */
/* Description: Gets the baud rate constant for the given number             */
/*              Wrapper function for seru_get_baud()                         */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type          Comments                                  */
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

extern speed_t serp_get_baud(unsigned long baud_num);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_baud_str()                                                 */
/*                                                                           */
/* Description: Gets the baud rate string for the given baud rate            */
/*              Wrapper function for seru_get_baud_str()                     */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        ----------------------------------   */
/*   baud           speed_t             The baud rate                        */
/*   baud_str       serp_baud_str_t     The buffer for the string            */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/* Pre-conditions:                                                           */
/*                                                                           */
/* Post-conditions: String returned in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern serp_baud_str_status_t serp_get_baud_str(speed_t baud,
  serp_baud_str_t baud_str);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_flush_port()                                                   */
/*                                                                           */
/* Description: Flush the port input and output buffers.                     */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        ----------------------------------   */
/*   fd             int                 The file descriptor of the port      */
/*   diags          bool                Detailed diagnostic messages flag    */
/*                                                                           */
/* Returns: The status of the flush, success or failure                      */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: The port buffers will be flushed.                        */
/*                                                                           */
/*****************************************************************************/

extern int serp_flush_port(int fd, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_port_config()                                              */
/*                                                                           */
/* Description: Set the configuration of the port.                           */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   req_setup      serp_port_setup_t   The requested setup for the port     */
/*   diags          bool                detailed diagnostic messages flag    */
/*                                                                           */
/* Returns: The status of the configuration, success or failure              */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port configured, unless error occured                    */
/*                                                                           */
/*****************************************************************************/

extern int serp_set_port_config(struct serp_port_setup_t *req_setup,
  bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_restore_port_state()                                           */
/*                                                                           */
/* Description: Restore the serial port settings.                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open & port state saved                              */
/*                                                                           */
/* Post-conditions: Port state restored                                      */
/*                                                                           */
/*****************************************************************************/

extern void serp_restore_port_state(int fd, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_save_port_state()                                              */
/*                                                                           */
/* Description: Save the serial port settings.                               */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns:Status indicating success or failure                              */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port state saved                                         */
/*                                                                           */
/*****************************************************************************/

extern int serp_save_port_state(int fd, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_lock_port()                                                    */
/*                                                                           */
/* Description: Locks a serial port.                                         */
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

extern serp_lock_status_t serp_lock_port(char *serial_port);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_unlock_port()                                                  */
/*                                                                           */
/* Description: Unlocks a serial port.                                       */
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

extern void serp_unlock_port(char *serial_port);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_open_port()                                                    */
/*                                                                           */
/* Description: Opens serial port.                                           */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   serial_port    char *          The serial port to use                   */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: File descriptor, or status indicating failure                    */
/*                                                                           */
/* Pre-conditions: Port closed                                               */
/*                                                                           */
/* Post-conditions: Port open, unless error occured                          */
/*                                                                           */
/*****************************************************************************/

extern int serp_open_port(char *serial_port, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_close_port()                                                   */
/*                                                                           */
/* Description: Close serial port.                                           */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: Void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Port closed                                              */
/*                                                                           */
/*****************************************************************************/

extern void serp_close_port(int fd, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_lines()                                                    */
/*                                                                           */
/* Description: Get a serial port's control lines                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name         Type              Comments                                 */
/*   -----------  ------------      -----------------------------------      */
/*   get_lines    serp_get_lines_t  Ptr to struct with params & for results  */
/*   diags        bool              Detailed diagnostic messages required    */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Returned control lines status in buffer                  */
/*                                                                           */
/*****************************************************************************/

extern void serp_get_lines(serp_get_lines_t *get_lines, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_lines()                                                    */
/*                                                                           */
/* Description: Set a serial port's control lines                            */
/*                                                                           */
/* Parameters:                                                               */
/*   Name        Type              Comments                                  */
/*   ----------  ------------      -----------------------------------       */
/*   set_lines   serp_set_lines_t  Ptr to struct with params & for results   */
/*   diags       bool              Detailed diagnostic messages required     */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Control lines altered & returned status in buffer        */
/*                                                                           */
/*****************************************************************************/

extern void serp_set_lines(serp_set_lines_t *set_lines, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_dtr_rts()                                                  */
/*                                                                           */
/* Description: Set a serial port's DTR and RTS control lines to on or off   */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type               Comments                             */
/*   ---------       ------------       -----------------------------------  */
/*   req_set_dtr_rts serp_set_dtr_rts_t Ptr to struct with params & results  */
/*   diags           bool               Detailed diagnostic msgs required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open                                                 */
/*                                                                           */
/* Post-conditions: Control lines altered & returned status in buffer        */
/*                                                                           */
/*****************************************************************************/

extern void serp_set_dtr_rts(serp_set_dtr_rts_t *req_set_dtr_rts, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_read_port()                                                    */
/*                                                                           */
/* Description: Read from the serial port.                                   */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   ret_rx_buf     serp_rx_buf_t   The buffer for returned byte & status    */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned byte and status in buffer                       */
/*                                                                           */
/*****************************************************************************/

extern void serp_read_port(serp_rx_buf_t *ret_rx_buf, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_wait_for_read()                                                */
/*                                                                           */
/* Description: Wait until a read from the serial port is ready, or timeout  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   read_timeout   serp_timeout_t  The read timeout                         */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: The status of the wait                                           */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern serp_rx_wait_status_t serp_wait_for_read(int fd,
  serp_timeout_t read_timeout, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_write_port()                                                   */
/*                                                                           */
/* Description: Write to the serial port.                                    */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   req_tx_buf     serp_tx_buf     Byte to send and status returned         */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: Void                                                             */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern void serp_write_port(serp_tx_buf_t *req_tx_buf, bool diags);


/*****************************************************************************/
/*                                                                           */
/* Name: serp_wait_for_write()                                               */
/*                                                                           */
/* Description: Wait until the serial port is ready to send a byte           */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   write_timeout  serp_timeout_t  The write timeout                        */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: The status of the wait                                           */
/*                                                                           */
/* Pre-conditions: Port open and configured                                  */
/*                                                                           */
/* Post-conditions: Returned status in buffer                                */
/*                                                                           */
/*****************************************************************************/

extern serp_tx_wait_status_t serp_wait_for_write(int fd,
 serp_timeout_t write_timeout, bool diags);


#endif /* SERP_H */

