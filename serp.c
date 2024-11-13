/*****************************************************************************/
/*                                                                           */
/* Module: serp.c                                                            */
/*                                                                           */
/* Description: Wrapper functions for seru, providing error messages         */
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


/*****************************************************************************/
/*      INCLUDED FILES (dependencies)                                        */
/*****************************************************************************/

#include <stdio.h>       /* Standard I/O definitions - fprintf()           */
#include <string.h>      /* Standard string definitions - strerror()       */
#include <stdbool.h>     /* Boolean types                                  */
#include <sys/time.h>    /* Standard time definitions - timeval            */
#include "seru.h"        /* Header file for the serial utils library       */
#include "serp.h"        /* Header file for this library                   */


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

/*****************************************************************************/
/*      INTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*      INTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: i_check_get_port_attribs()                                          */
/*                                                                           */
/* Description: Check if getting the port attributes failed, and print       */
/*              appropriate message                                          */
/*                                                                           */
/* Uses: setup - A pointer to the status for the port                        */
/*       diags - Detailed diagnostic messages flag                           */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_check_get_port_attribs(struct seru_port_setup_t *setup,
  bool diags)
{

  int config_status = SERP_PORT_SUCCESS; /* Status of configuring a port */
  char *errstr;                          /* The error string             */


  /* Did configure fail getting the port attributes? */
  if( (setup->status & SERU_STP_GET_OPTS_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Get port attributes error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to get port attributes\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_set_port_ispeed()                                           */
/*                                                                           */
/* Description: Check if setting the port input speed failed, and print      */
/*              appropriate message                                          */
/*                                                                           */
/* Uses: setup - A pointer to the status for the port                        */
/*       diags - Detailed diagnostic messages flag                           */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_check_set_port_ispeed(struct seru_port_setup_t *setup,
  bool diags)
{

  int config_status = SERP_PORT_SUCCESS; /* Status of configuring a port */
  char *errstr;                          /* The error string             */


  /* Did configure fail setting the input baud rate? */
  if( (setup->status & SERU_STP_IBAUD_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Set input baud rate error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to set input baud rate\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_set_port_ospeed()                                           */
/*                                                                           */
/* Description: Check if setting the port output speed failed, and print     */
/*              appropriate message                                          */
/*                                                                           */
/* Uses: setup - A pointer to the status for the port                        */
/*       diags - Detailed diagnostic messages flag                           */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_check_set_port_ospeed(struct seru_port_setup_t *setup,
  bool diags)
{

  int config_status = SERP_PORT_SUCCESS; /* Status of configuring a port */
  char *errstr;                          /* The error string             */


  /* Did configure fail setting the output baud rate? */
  if( (setup->status & SERU_STP_OBAUD_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Set output baud rate error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to set output baud rate\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_set_port_options()                                          */
/*                                                                           */
/* Description: Check if setting the port options failed, and print          */
/*              appropriate message                                          */
/*                                                                           */
/* Uses: setup - A pointer to the status for the port                        */
/*       diags - Detailed diagnostic messages flag                           */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_check_set_port_options(struct seru_port_setup_t *setup,
  bool diags)
{

  int config_status = SERP_PORT_SUCCESS; /* Status of configuring a port */
  char *errstr;                          /* The error string             */


  /* Did configure fail setting the port attributes? */
  if( (setup->status & SERU_STP_SET_OPTS_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Set port attributes error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to set port attributes\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_set_low_latency()                                           */
/*                                                                           */
/* Description: Check if setting low latency failed, and print               */
/*              appropriate message                                          */
/*                                                                           */
/* Uses: setup - A pointer to the status for the port                        */
/*       diags - Detailed diagnostic messages flag                           */
/*                                                                           */
/* Returns: Success or failure indication                                    */
/*                                                                           */
/*****************************************************************************/

static int i_check_set_low_latency(struct seru_port_setup_t *setup,
  bool diags)
{

  int config_status = SERP_PORT_SUCCESS; /* Status of configuring a port */
  char *errstr;                          /* The error string             */


  /* Did configure fail setting the port attributes? */
  if( (setup->status & SERU_STP_IOCTL_GET_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Set port error: ioctl get: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to set low latency\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  /* Did configure fail setting the port attributes? */
  if( (setup->status & SERU_STP_IOCTL_SET_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(setup->setup_errno);

      fprintf(stderr,"Set port error: ioctl set: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Could not set low latency\n");

    }

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}

/*****************************************************************************/
/*      EXTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*      EXTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_timeout()                                                  */
/*                                                                           */
/* Description: Gets the timeout for the current baud rate                   */
/*              Wrapper function for seru_get_timeout()                      */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        ----------------------------------   */
/*   baud           speed_t             The baud rate                        */
/*                                                                           */
/* Returns: The timeout, or a failure indication                             */
/*                                                                           */
/*****************************************************************************/

extern serp_timeout_t serp_get_timeout(speed_t baud)
{

  serp_timeout_t timeout; /* The timeout */


  timeout = seru_get_timeout(baud);

  if(timeout == SERU_GET_TIMEOUT_FAIL)
  {

    fprintf(stderr,"Unable to get timeout\n");

  }

  return timeout;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_baud()                                                     */
/*                                                                           */
/* Description: Gets the baud rate constant for the given number             */
/*              Wrapper function for seru_get_baud()                         */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type           Comments                                 */
/*   ------------    ------------  -----------------------------------       */
/*   baud_num        unsigned long A number to convert to a baud rate        */
/*                                                                           */
/* Returns: The baud rate, or a failure indication                           */
/*                                                                           */
/*****************************************************************************/

extern speed_t serp_get_baud(unsigned long baud_num)
{

  speed_t baud;                  /* The baud rate constant                */


  baud = seru_get_baud(baud_num);

  if(baud == SERU_GET_BAUD_FAIL)
  {

    fprintf(stderr,"Baud rate invalid\n");

  }

  /* Return baud or fail status */
  return baud;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_baud_str()                                                 */
/*                                                                           */
/* Description: Gets the baud rate string for the given baud rate            */
/*              Wrapper function for seru_get_baud_str()                     */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   baud           speed_t             The baud rate                        */
/*   baud_str       serp_baud_str_t     The buffer for the string            */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/*****************************************************************************/
extern serp_baud_str_status_t serp_get_baud_str(speed_t baud,
  serp_baud_str_t baud_str)
{

  seru_baud_str_status_t baud_str_status; /* The 'get' status          */


  /* Get the string for the baud rate */
  baud_str_status = seru_get_baud_str(baud, baud_str);

  if(baud_str_status == SERU_GET_BAUD_STR_FAIL)
  {

    fprintf(stderr,"Invalid baud rate\n");

  }

  return (serp_baud_str_status_t) baud_str_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_flush_port()                                                   */
/*                                                                           */
/* Description: Flush the port input and output buffers. Wrapper function    */
/* for seru_flush_port()                                                     */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   fd             int                 The file descriptor of the port      */
/*   diags          bool                Detailed diagnostic messages flag    */
/*                                                                           */
/* Returns: The status of the flush, success or failure                      */
/*                                                                           */
/*****************************************************************************/

extern int serp_flush_port(int fd, bool diags)
{

  int flush_status = SERP_PORT_SUCCESS; /* Status of flushing a port        */
  char *errstr;                         /* The error string                 */
  struct seru_port_flush_t flush;       /* The params & status for flushing */


  /* Set the file descriptor */
  flush.fd = fd;

  /* Clear the status */
  flush.status = SERU_FLUSH_INIT;

  /* Flush the port */
  seru_flush_port(&flush);

  /* Did flushing the port fail? */
  if(flush.status == SERU_FLUSH_FAIL)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(flush.flush_errno);

      fprintf(stderr,"Flush port error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to flush the port\n");

    }

    flush_status = SERP_PORT_FAILURE;

  }

  return flush_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_port_config()                                              */
/*                                                                           */
/* Description: Set the configuration of the port.  Wrapper function for     */
/*              seru_set_port_config()                                       */
/*                                                                           */
/* Internal functions used: i_check_get_port_attribs()                       */
/*                          i_check_set_port_ispeed()                        */
/*                          i_check_set_port_ospeed()                        */
/*                          i_check_set_port_options()                       */
/*                          i_check_set_low_latency()                        */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type                Comments                             */
/*   ------------   ------------        -----------------------------------  */
/*   req_setup      serp_port_setup_t   The requested setup for the port     */
/*   diags          bool                Detailed diagnostic messages flag    */
/*                                                                           */
/* Returns: The status of the configuration, success or failure              */
/*                                                                           */
/*****************************************************************************/

extern int serp_set_port_config(struct serp_port_setup_t *req_setup,
  bool diags)
{

  int get_attribs_status = SERP_PORT_SUCCESS;
                              /* Status of getting port attributes       */
  int set_ispeed_status = SERP_PORT_SUCCESS;
                              /* Status of setting port input speed      */
  int set_ospeed_status = SERP_PORT_SUCCESS;
                              /* Status of setting port output speed     */
  int set_options_status = SERP_PORT_SUCCESS;
                              /* Status of setting port options          */
  int set_low_latency_status = SERP_PORT_SUCCESS;
                              /* Status of setting low latency           */
  int config_status = SERP_PORT_SUCCESS;
                              /* Status of configuring a port            */
  struct seru_port_setup_t setup;
                              /* The structure for setup data and status */


  /* Transfer the configuration from the requested to seru structs */
  setup.fd = req_setup->fd;

  setup.speed = req_setup->speed;

  setup.data_bits = (seru_data_bits_t) req_setup->data_bits;

  setup.parity = (seru_parity_t) req_setup->parity;

  setup.stop_bits = (seru_stop_bits_t) req_setup->stop_bits;

  setup.low_latency = (bool) req_setup->low_latency;

  /* Clear the status */
  setup.status = SERU_STP_INIT;

  /* Configure the port */
  seru_set_port_config(&setup);

  /* Did configure fail getting the port attributes? */
  get_attribs_status = i_check_get_port_attribs(&setup, diags);

  /* Did configure fail setting the input baud rate? */
  set_ispeed_status = i_check_set_port_ispeed(&setup, diags);

  /* Did configure fail setting the output baud rate? */
  set_ospeed_status = i_check_set_port_ospeed(&setup, diags);

  /* Did configure fail setting the port options? */
  set_options_status = i_check_set_port_options(&setup, diags);

  /* Did configure fail setting low latency? */
  set_low_latency_status = i_check_set_low_latency(&setup, diags);

  /* Were valid data bits given? */
  if( (setup.status & SERU_STP_BAD_DATA_BITS) > 0)
  {

    fprintf(stderr,"Invalid data bits\n");

    config_status = SERP_PORT_FAILURE;

  }

  /* Was a valid parity given? */
  if( (setup.status & SERU_STP_BAD_PARITY) > 0)
  {

    fprintf(stderr,"Invalid parity\n");

    config_status = SERP_PORT_FAILURE;

  }

  /* Were valid stop bits given? */
  if( (setup.status & SERU_STP_BAD_STOP_BITS) > 0)
  {

    fprintf(stderr,"Invalid stop bits\n");

    config_status = SERP_PORT_FAILURE;

  }

  if( (get_attribs_status == SERP_PORT_FAILURE)  ||
      (set_ispeed_status == SERP_PORT_FAILURE)   ||
      (set_ospeed_status == SERP_PORT_FAILURE)   ||
      (set_options_status == SERP_PORT_FAILURE)  ||
      (set_low_latency_status == SERP_PORT_FAILURE ) )
  {

    config_status = SERP_PORT_FAILURE;

  }

  return config_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_restore_port_state()                                           */
/*                                                                           */
/* Description: Restore the serial port settings.  Wrapper function for      */
/*              seru_restore_port_state()                                    */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_restore_port_state(int fd, bool diags)
{

  int status = SERP_PORT_SUCCESS;     /* Status of restoring port, OK/fail  */
  seru_restore_port_t restore_params; /* The request params & return status */
  char *errstr;                       /* The error string                   */


  /* Set the file descriptor */
  restore_params.fd = fd;

  status = seru_restore_port_state(&restore_params);

  if(status == SERU_PORT_FAILURE)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(restore_params.restore_errno);

      fprintf(stderr,"Restore port state error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to restore port state\n");

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_save_port_state()                                              */
/*                                                                           */
/* Description: Save the serial port settings.  Wrapper function for         */
/*              seru_save_port_state()                                       */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns:Status indicating success or failure                              */
/*                                                                           */
/*****************************************************************************/

extern int serp_save_port_state(int fd, bool diags)
{

  int status = SERP_PORT_SUCCESS; /* Status of saveing port, OK or fail   */
  seru_save_port_t save_params;   /* The requested params & return status */
  char *errstr;                   /* The error string                     */


  /* Set the file descriptor */
  save_params.fd = fd;

  status = seru_save_port_state(&save_params);

  if(status == SERU_PORT_FAILURE)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(save_params.save_errno);

      fprintf(stderr,"Save port state error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to save port state\n");

    }

  }

  return status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_lock_port()                                                    */
/*                                                                           */
/* Description: Locks a serial port. Wrapper function for seru_lock_port()   */
/*                                                                           */
/* Internal functions used:                                                  */
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

extern serp_lock_status_t serp_lock_port(char *serial_port)
{

  seru_lock_status_t lock_port_return; /* Return value from lock function */
  serp_lock_status_t lock_status;      /* The status of the lock          */


  lock_port_return = seru_lock_port(serial_port);

  switch (lock_port_return)
  {

    case SERU_LOCKED : /* The port was already locked */

      fprintf(stderr, "Serial port locked by another program\n");

      lock_status = SERP_LOCK_FAIL;

      break;

    case SERU_LOCK_FAIL : /* Failed to lock the port */

      fprintf(stderr, "Unable to lock serial port\n");

      lock_status = SERP_LOCK_FAIL;

      break;

    case SERU_LOCK_OK : /* The port was locked sucessfully */

      lock_status = SERP_LOCK_OK;

      break;

  } /* End switch */

  return lock_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_unlock_port()                                                  */
/*                                                                           */
/* Description: Unlocks a serial port. Wrapper function for                  */
/*  seru_unlock_port()                                                       */
/*                                                                           */
/* Internal functions used:                                                  */
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

extern void serp_unlock_port(char *serial_port)
{

  seru_unlock_port(serial_port);

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_open_port()                                                    */
/*                                                                           */
/* Description: Opens serial port. Wrapper function for seru_open_port()     */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   serial_port    char *          The serial port to use                   */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: File descriptor, or status indicating failure                    */
/*                                                                           */
/*****************************************************************************/

extern int serp_open_port(char *serial_port, bool diags)
{

  seru_port_open_t open_params;   /* The requested params & return status */
  int fd;                         /* The file descriptor                  */
  char *errstr;                   /* The error string                     */


  /* Clear status */
  open_params.open_status = SERU_OPEN_INIT;

  /* Open the serial port */
  seru_open_port(serial_port, &open_params);

  /* Did an error occur while opening the port (open)? */
  if( (open_params.open_status & SERU_OPEN_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(open_params.open_errno);

      fprintf(stderr,"Open port error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr, "Unable to open port\n");

    }

  }

  /* Did an error occur while opening the port (fcntl get)? */
  if( (open_params.open_status & SERU_OPEN_FCNTL_GET_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(open_params.open_errno);

      fprintf(stderr,"Open port error: fcntl get: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Cannot open port\n");

    }

  }

  /* Did an error occur while opening the port (fcntl set)? */
  if( (open_params.open_status & SERU_OPEN_FCNTL_SET_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(open_params.open_errno);

      fprintf(stderr,"Open port error: fcntl set: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Port cannot be opened\n");

    }

  }


  fd = open_params.fd;

  return fd;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_close_port()                                                   */
/*                                                                           */
/* Description: Close serial port. Wrapper function for seru_close_port()    */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type            Comments                                 */
/*   ------------   ------------    -----------------------------------      */
/*   fd             int             The file descriptor of the port          */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_close_port(int fd, bool diags)
{

  seru_port_close_t close_params; /* The requested params & return status */
  char *errstr;                   /* The error string                     */


  /* Clear status */
  close_params.close_status = SERU_CLOSE_INIT;

  /* Set the file descriptor */
  close_params.fd = fd;

  /* Close the serial port */
  seru_close_port(&close_params);

  /* Did an error occur while closing the port? */
  if( (close_params.close_status & SERU_CLOSE_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(close_params.close_errno);

      fprintf(stderr,"Close port error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to close port\n");

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_get_lines()                                                    */
/*                                                                           */
/* Description: Get a serial port's control lines                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name          Type              Comments                                */
/*   ------------  ------------      -----------------------------------     */
/*   req_get_lines serp_get_lines_t  Ptr to struct with params & for results */
/*   diags         bool              Detailed diagnostic msgs required?      */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_get_lines(serp_get_lines_t *req_get_lines, bool diags)
{

  seru_get_lines_t get_lines; /* Struct for control lines & status */
  char *errstr;         /* The error string                  */


  /* Reset status */
  get_lines.status = SERU_GET_LINES_INIT;

  /* Clear errno */
  get_lines.get_errno = 0;

  /* Transfer file descriptor between structures */
  get_lines.fd = req_get_lines->fd;

  /* Get the control lines */
  seru_get_lines(&get_lines);

  /* Did get fail? */
  if( (get_lines.status & SERU_GET_LINES_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(get_lines.get_errno);

      fprintf(stderr,"Get control lines error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to get control lines\n");

    }

  }

  /* Pass back the status & control lines*/

  req_get_lines->status = (serp_get_line_status_t) get_lines.status;

  req_get_lines->lines = get_lines.lines;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_lines()                                                    */
/*                                                                           */
/* Description: Set a serial port's control lines                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name          Type             Comments                                 */
/*   ------------  ------------     -----------------------------------      */
/*   req_set_lines serp_set_lines_t Ptr to struct with params & for results  */
/*   diags          bool            Detailed diagnostic messages required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_set_lines(serp_set_lines_t *req_set_lines, bool diags)
{

  seru_set_lines_t set_lines; /* Struct for required control lines & status */
  char *errstr;               /* The error string                           */


  /* Reset status */
  set_lines.status = SERU_SET_LINES_INIT;

  /* Clear errno */
  set_lines.set_errno = 0;

  /* Transfer file descriptor between structures */
  set_lines.fd = req_set_lines->fd;

  /* Transfer required control line data */
  set_lines.lines = req_set_lines->lines;

  /* Set the control lines */
  seru_set_lines(&set_lines);

  /* Did set fail? */
  if( (set_lines.status & SERU_SET_LINES_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(set_lines.set_errno);

      fprintf(stderr,"Set control lines error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to set control lines\n");

    }

  }

  /* Pass back the status */
  req_set_lines->status = (serp_set_line_status_t) set_lines.status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_set_dtr_rts()                                                  */
/*                                                                           */
/* Description: Set a serial port's DTR and RTS control lines to on or off   */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name            Type               Comments                             */
/*   ---------       ------------       -----------------------------------  */
/*   req_set_dtr_rts serp_set_dtr_rts_t Ptr to struct with params & results  */
/*   diags           bool               Detailed diagnostic msgs required?   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_set_dtr_rts(serp_set_dtr_rts_t *req_set_dtr_rts, bool diags)
{

  seru_set_dtr_rts_t set_dtr_rts; /* Struct for setting DTR AND RTS */
  char *errstr;                   /* The error string               */


  /* Reset get control lines status */
  set_dtr_rts.get_status = SERU_GET_LINES_INIT;

  /* Clear get control lines errno */
  set_dtr_rts.get_errno = 0;

  /* Reset get control lines status */
  set_dtr_rts.set_status = SERU_SET_LINES_INIT;

  /* Clear get control lines errno */
  set_dtr_rts.set_errno = 0;

  /* Transfer file descriptor between structures */
  set_dtr_rts.fd = req_set_dtr_rts->fd;

  /* Transfer required control line data */
  set_dtr_rts.req_lines = req_set_dtr_rts->req_lines;

  /* Set control lines */
  seru_set_dtr_rts(&set_dtr_rts);

  /* Did get fail? */
  if( (set_dtr_rts.get_status & SERU_GET_LINES_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(set_dtr_rts.get_errno);

      fprintf(stderr,"Get control lines error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to get control lines\n");

    }

  }
  else
  {
    /* Was an invalid line state requested? */
    if( (set_dtr_rts.set_status & SERU_SET_LINES_INVALID) > 0)
    {

      fprintf(stderr,"Invalid control line state requested\n");

    }
    else
    {

      /* Did set fail? */
      if( (set_dtr_rts.set_status & SERU_SET_LINES_FAILURE) > 0)
      {

        /* Do we want a detailed error report? */
        if(diags == true)
        {

          errstr = strerror(set_dtr_rts.set_errno);

          fprintf(stderr,"Set control lines error: %s\n", errstr);

        }
        else
        {

          fprintf(stderr,"Unable to set control lines\n");

        }

      }

    }

  }

  /* Don't pass back everything, just a simple ok/failed status */

  /* Pass back the get status */
  req_set_dtr_rts->get_status = (serp_get_line_status_t) set_dtr_rts.get_status;

  req_set_dtr_rts->get_errno = (serp_get_line_status_t) set_dtr_rts.get_errno;

  /* Pass back the set status */
  req_set_dtr_rts->set_status = (serp_set_line_status_t) set_dtr_rts.set_status;

  req_set_dtr_rts->set_errno = (serp_set_line_status_t) set_dtr_rts.set_errno;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_read_port()                                                    */
/*                                                                           */
/* Description: Read from the serial port.  Wrapper function for             */
/*              seru_read_serial()                                           */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name          Type            Comments                                  */
/*   ------------  ------------    -----------------------------------       */
/*   ret_rx_buf    serp_rx_buf_t   The buffer for returned byte & status     */
/*   diags         bool            Are detailed diagnostic messages required */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_read_port(serp_rx_buf_t *ret_rx_buf, bool diags)
{

  seru_rx_buf_t rx_buf; /* Struct for received byte & status */
  char *errstr;         /* The error string                  */


  /* Clear status */
  rx_buf.rx_status = SERU_READ_INIT;

  /* Clear errno */
  rx_buf.rx_errno = 0;

  /* Clear time errno */
  rx_buf.time_errno = 0;

  /* Clear RX time */
  rx_buf.rx_time.tv_sec = 0;

  rx_buf.rx_time.tv_usec = 0;

  /* Transfer file descriptor between structures */
  rx_buf.fd = ret_rx_buf->fd;

  /* Read byte from serial port */
  seru_read_port(&rx_buf);

  /* Did read fail? */
  if( (rx_buf.rx_status & SERU_READ_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(rx_buf.rx_errno);

      fprintf(stderr,"Read port error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to read from port\n");

    }

  }

  /* Did Time read fail? */
  if( (rx_buf.rx_status & SERU_READ_TIME_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if( (diags == true) && (rx_buf.time_errno != 0) )
    {

      errstr = strerror(rx_buf.time_errno);

      fprintf(stderr,"Read time error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to read time\n");

    }

  }

  /* Pass back the byte, status & time */

  ret_rx_buf->rx_status = (serp_rx_status_t) rx_buf.rx_status;

  ret_rx_buf->rx_byte = rx_buf.rx_byte;

  ret_rx_buf->rx_time.tv_sec = rx_buf.rx_time.tv_sec;

  ret_rx_buf->rx_time.tv_usec = rx_buf.rx_time.tv_usec;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_wait_for_read()                                                */
/*                                                                           */
/* Description: Wait until a read from the serial port is ready, or timeout  */
/*              Wrapper function for seru_wait_for_read()                    */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name          Type            Comments                                  */
/*   ------------  ------------    -----------------------------------       */
/*   fd            int             The file descriptor of the port           */
/*   read_timeout  serp_timeout_t  The read timeout                          */
/*   diags         bool            Are detailed diagnostic messages required */
/*                                                                           */
/* Returns: The status of the wait                                           */
/*                                                                           */
/*****************************************************************************/

extern serp_rx_wait_status_t serp_wait_for_read(int fd,
  serp_timeout_t read_timeout, bool diags)
{

  seru_rx_wait_t rx_wait;  /* Struct for wait params & status */
  char *errstr;            /* The error string                */


  /* Clear status */
  rx_wait.rx_wait_status = SERU_RX_WAIT_INIT;

  /* Transfer file descriptor between structures */
  rx_wait.fd = fd;

  /* Pass the timeout */
  rx_wait.read_timeout = (seru_timeout_t) read_timeout;

  /* Wait until we are ready to read, or timeout */
  seru_wait_for_read(&rx_wait);

  if( (rx_wait.rx_wait_status & SERU_RX_WAIT_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(rx_wait.rx_wait_errno);

      fprintf(stderr,"Wait for read error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Failed wait for read\n");

    }

  }

  return (serp_rx_wait_status_t) rx_wait.rx_wait_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_write_port()                                                   */
/*                                                                           */
/* Description: Write to the serial port. Wrapper function for               */
/*              seru_write_port()                                            */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type           Comments                                  */
/*   ------------   ------------   -----------------------------------       */
/*   req_tx_buf     serp_tx_buf    Byte to send and status returned          */
/*   diags          bool           Are detailed diagnostic messages required */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

extern void serp_write_port(serp_tx_buf_t *req_tx_buf, bool diags)
{

  seru_tx_buf_t tx_buf; /* Struct for transmit byte & returned status */
  char *errstr;         /* The error string                           */


  /* Put the byte to transmit in the TX buffer */
  tx_buf.tx_byte = req_tx_buf->tx_byte;

  /* Clear the status */
  tx_buf.tx_status = SERU_WRITE_INIT;

  /* Clear time errno */
  tx_buf.time_errno = 0;

  /* Clear RX time */
  tx_buf.tx_time.tv_sec = 0;

  tx_buf.tx_time.tv_usec = 0;

  /* Transfer file descriptor between structures */
  tx_buf.fd = req_tx_buf->fd;

  /* Write byte to serial port */
  seru_write_port(&tx_buf);

  /* Was write successful? */
  if( (tx_buf.tx_status & SERU_WRITE_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(tx_buf.tx_errno);

      fprintf(stderr,"Write port error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to write to port\n");

    }

  }

  /* Did Time read fail? */
  if( (tx_buf.tx_status & SERU_WRITE_TIME_FAIL) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(tx_buf.time_errno);

      fprintf(stderr,"Read time error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Unable to read time\n");

    }

  }

  /* Return the status & time */
  req_tx_buf->tx_status = (serp_tx_status_t) tx_buf.tx_status;

  req_tx_buf->tx_time.tv_sec = tx_buf.tx_time.tv_sec;

  req_tx_buf->tx_time.tv_usec = tx_buf.tx_time.tv_usec;

}


/*****************************************************************************/
/*                                                                           */
/* Name: serp_wait_for_write()                                               */
/*                                                                           */
/* Description: Wait until the serial port is ready to send a byte           */
/*              Wrapper function for seru_wait_for_write()                   */
/*                                                                           */
/* Internal functions used:                                                  */
/*                                                                           */
/* Internal variables used:                                                  */
/*                                                                           */
/* Parameters:                                                               */
/*   Name           Type           Comments                                  */
/*   ------------   ------------   -----------------------------------       */
/*   fd             int            The file descriptor of the port           */
/*   write_timeout  serp_timeout_t The write timeout                         */
/*   diags          bool           Are detailed diagnostic messages required */
/*                                                                           */
/* Returns: The status of the wait                                           */
/*                                                                           */
/*****************************************************************************/

extern serp_tx_wait_status_t serp_wait_for_write(int fd,
 serp_timeout_t write_timeout, bool diags)
{

  seru_tx_wait_t tx_wait;  /* Struct for wait params & status */
  char *errstr;            /* The error string                */


  /* Clear status */
  tx_wait.tx_wait_status = SERU_TX_WAIT_INIT;

  /* Transfer file descriptor between structures */
  tx_wait.fd = fd;

  /* Pass the timeout */
  tx_wait.write_timeout = (seru_timeout_t) write_timeout;

  /* Wait until we are ready to write, or timeout */
  seru_wait_for_write(&tx_wait);

  /* Report any errors */

  if( (tx_wait.tx_wait_status & SERU_TX_WAIT_FAILURE) > 0)
  {

    /* Do we want a detailed error report? */
    if(diags == true)
    {

      errstr = strerror(tx_wait.tx_wait_errno);

      fprintf(stderr,"Wait for write error: %s\n", errstr);

    }
    else
    {

      fprintf(stderr,"Failed wait for write\n");

    }

  }
  else
  {

     /* Or did we timeout */
    if( (tx_wait.tx_wait_status & SERU_TX_WAIT_TIMEOUT) > 0)
    {

      fprintf(stderr,"Unable to write to port\n");

    }

  }

  return (serp_tx_wait_status_t) tx_wait.tx_wait_status;

}
