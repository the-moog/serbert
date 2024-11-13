/*****************************************************************************/
/*                                                                           */
/* Module: serbert.c                                                         */
/*                                                                           */
/* Description: Bit Error Rate Test (BERT) for serial ports                  */
/*                                                                           */
/* Copyright (C) 2004, 2010, 2011       - David Wilson Clarke                */
/*  Resurected with minor changes 2024  - Jay Morgan (the-moog)              */
/*                                                                           */
/* This file is part of Serbert. v0.3.1  https://github.com/the-moog/serbert */
/*                               v0.3.0  https://serbert.sourceforge.com     */
/*                                                                           */
/* Serbert is free software; you can redistribute it and/or modify           */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* any later version.                                                        */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License v2.0    */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*      INCLUDED FILES (dependencies)                                        */
/*****************************************************************************/

#include <stdio.h>       /* Standard I/O definitions  - printf(), fprintf() */
#include <string.h>      /* String manipulation lib - strlen(), memmove()   */
#include <stdlib.h>      /* Standard library - atoi(), strtoul(), rand()    */
#include <ctype.h>       /* Character tests - isxdigit(), tolower()         */
//#include <time.h>        /* Time defs - time(), localtime(), strftime()     */
//                         /* nanosleep()                                     */
#include <sys/time.h>    /* Standard time definitions - timeval, timersub   */
#include <limits.h>      /* Variable max sizes - ULONG_MAX                  */
#include <stdbool.h>     /* Boolean types                                   */
#include <fcntl.h>       /* Fcntl types - fcntl()                           */
#include <errno.h>       /* Provides errno                                  */
#include <math.h>        /* Provides HUGE_VAL                               */
#include "serp.h"        /* Serial utilities library                        */
#include "serbert_config.h"
                         /* Compile time configuration options for Serbert  */


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

/* Type for argument status */
typedef enum arg_status_t
  {i_ARG_INVALID, i_ARG_VALID} arg_status_t;

/* Pointer to function typedef used by i_select_function */
typedef arg_status_t (*i_pvf_arg_func_t)(char *);

/* Pointer to function typedef used by i_argument_t */
typedef arg_status_t (*i_pvf_func_t)();

/* Type for Binary or Decimal selection */
typedef enum bin_not_dec_t {i_DEC, i_BIN} bin_not_dec_t;

/* Type for testing method */
typedef enum how_test_t
  {i_TEST_NUM = 1, i_TEST_TIME, i_TEST_CONTINUOUS} how_test_t;


/* Enums */

enum { i_TIME_MAX = LONG_MAX };         /* Maximum value of a time_t        */

enum { i_KIBIBYTE = 0x400 };            /* One Kibibyte (2 ^ 10)            */

enum { i_MEBIBYTE = 0x100000 };         /* One Mebibyte (2 ^ 20)            */

enum { i_GIBIBYTE = 0x40000000 };       /* One Gibibyte (2 ^ 30)            */

enum { i_KILOBYTE = 1000 };             /* One kilobyte (10 ^ 3)            */

enum { i_MEGABYTE = 1000000 };          /* One megabyte (10 ^ 6)            */

enum { i_GIGABYTE = 1000000000 };       /* One gigabyte (10 ^ 9)            */

enum { i_SEC_IN_MIN = 60 };             /* Number of seconds in a minute    */

enum { i_SEC_IN_HOUR = i_SEC_IN_MIN * 60 };
                             /* Number of seconds in a minute               */

enum { i_MAX_ARG_LEN = 80 }; /* Command line argument max allowed length    */

enum { i_MAX_COMP_ARGS = 1 };
                             /* Number of compulsory command line arguments */

enum { i_TX_BUF_LEN = 300 }; /* TX buffer length                            */
/* This must be larger than i_NUM_CHARS */

enum { i_NUM_CHARS = 256 };  /* Default number of chars in TX buffer        */
/* i_TX_BUF_LEN must be larger than this */

enum { i_DEFAULT_TX_SIZE = 1024 };
                             /* Default number of bytes to send             */

enum { i_DEFAULT_BAUD_RATE = B19200 };
                             /* Default RX and TX baud rates                */

const serp_data_bits_t i_DEFAULT_DATA_BITS = SERP_8_DATA_BITS;
                             /* Default data bits                           */

const serp_parity_t i_DEFAULT_PARITY = SERP_NO_PARITY;
                             /* Default parity                              */

const serp_stop_bits_t i_DEFAULT_STOP_BITS = SERP_1_STOP_BIT;
                             /* Default stop bits                           */

enum { i_DEFAULT_WRITE_TIMEOUT = 50 };
                             /* Default write timeout in microseconds       */

const bin_not_dec_t i_DEFAULT_BIN_NOT_DEC = i_DEC;
                             /* Default binary or decimal multiplier        */

enum { i_TIM_STR_LEN = 50 }; /* Max length of any time strings              */

enum { i_TIME_FAIL = -1 };   /* Time read failure state                     */

enum { i_STDIN_FAIL = -1 };  /* Failure setting stdin settings state        */

enum { i_EXIT_FAULT = 1 };   /* Flag fault on exit                          */

enum { i_EXIT_OK = 0 };      /* Flag all went well on exit                  */

static const double i_ESCAPE_TIME = 0.1;
                             /* Chunks size to break up pauses              */

enum { i_TERMINAL_WIDTH = 80 };
                             /* Width of terminal in chars                  */

enum { i_CHARS_PER_LINE_HEX8 = i_TERMINAL_WIDTH / 3 };
                             /* Number of 8 bit Hex chars per terminal line */

enum { i_STR_TERM = 0 };      /* String termination character               */

/* String literals */

/* Default serial port */
static const char *i_VERSION = "Serbert version 0.3.1";

/* Structs */

/* Command line arguments parameters */
typedef struct i_argument_t
{

  char arg;                  /* The argument character                  */
  i_pvf_func_t function;     /* The function to process the arg         */
  int options;               /* The number of options for this argument */
} i_argument_t;


/*****************************************************************************/
/*      INTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/

static int i_fd;                              /* Port file descriptor        */

static char i_serial_port[i_MAX_ARG_LEN + 1]; /* Serial port to use          */

static unsigned char i_tx_buf[i_TX_BUF_LEN];  /* Buffer for string to send   */

static unsigned int i_tx_ptr;                 /* Pointer to the byte to send */

static unsigned long long i_tx_len;           /* No of bytes to send         */

static unsigned int i_str_len;                /* Length of string to send    */

static speed_t i_baud_rate;                   /* Baud rate to use            */

static unsigned long long i_bytes_sent;       /* No. of bytes sent           */

static unsigned long long i_num_errors;       /* No. of errors during test   */

static unsigned long long i_num_timeouts;     /* No. of timeout errors       */

static unsigned long long i_num_corrupts;     /* No. of corrupt bytes        */

static serp_timeout_t i_read_timeout;         /* Read timeout in microseconds*/

static time_t i_start_time;                   /* The time the test started   */

static time_t i_send_time;                    /* The required time to send   */

static time_t i_runtime;                      /* The time the test took      */

static time_t i_intermediate_time;            /* When to show interm. results*/

static time_t i_last_results;                 /* Last time interm. shown     */

static bool i_user_set_timeout;               /* 'User has set timeout'flag  */

static bool i_verbose;                        /* Verbose mode flag           */

static bool i_quiet;                          /* Quiet mode flag             */

static bool i_diags;                          /* Diagnostic mode flag        */

static bool i_random;                         /* Send random bytes mode      */

static bool i_intermediate;                   /* Report intermediate results */

static bool i_show_stats;                     /* Show stats at end of run    */

static bool i_low_latency;                    /* Use low latency             */

static bin_not_dec_t i_bin_not_dec;           /* Binary or dec multipliers   */

static how_test_t i_how_test;                 /* Indicator of how to test    */

static bool i_q_pressed;                      /* 'q' key has been pressed    */

static bool i_i_pressed;                      /* 'i' key has been pressed    */

static struct termios i_saved_options;        /* Saved stdin options         */

static int i_saved_flags;                     /* Saved stdin flags           */

static double i_paced_time;                   /* Time to pause between chars */

static struct timeval i_tx_time;   /* The time the byte was transmitted      */

static struct timeval i_delta_time_max;   /* The max byte turnround time     */

static struct timeval i_delta_time_min;   /* The min byte turnround time     */

static struct timeval i_delta_time_av;    /* The average byte turnround time */


/*****************************************************************************/
/*      INTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Name: i_print_version()                                                   */
/*                                                                           */
/* Description: Print the program version number                             */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_version(void)
{

  printf("%s", i_VERSION);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_big_num()                                                   */
/*                                                                           */
/* Description: Print an unsigned long long value                            */
/*                                                                           */
/* Uses: number      - The number to print                                   */
/*       bin_not_dec - Are binary or decimal multipliers used (1000 or 1024) */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_big_num(unsigned long long number,
                            bin_not_dec_t bin_not_dec)
{

  unsigned long kilobyte;  /* The value for 1 kilobyte     */
  unsigned long megabyte;  /* The value for 1 megabyte     */
  unsigned long gigabyte;  /* The value for 1 gigabyte     */


  if(bin_not_dec == i_BIN)
  {

    kilobyte = i_KIBIBYTE;

    megabyte = i_MEBIBYTE;

    gigabyte = i_GIBIBYTE;

  }
  else
  {

    kilobyte = i_KILOBYTE;

    megabyte = i_MEGABYTE;

    gigabyte = i_GIGABYTE;

  }

  /* Is it over a gig? */
  if(number >= gigabyte)
  {

    printf("%.6gG", (double) number / gigabyte);

  }
  else
  {

    /* Is it over a meg? */
    if(number >= megabyte)
    {

      printf("%.6gM", (double) number / megabyte);

    }
    else
    {

      /* Is it over 10k? ie display 9999 then 10k */
      if(number >= ( 10 * kilobyte) )
      {

        printf("%.6g", (double) number / kilobyte);

        /*Set correct IEC postfix */
        if(bin_not_dec == i_BIN)
        {

           printf("K");

        }
        else
        {

           printf("k");

        }

      }
      else
      {

        printf("%llu", number);

      }

    }

  }

  /* Add IEC 'i' for binary multiples */
  if( (bin_not_dec == i_BIN) && (number >= ( 10 * kilobyte) ) )
  {

    printf("i");

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_date_n_time()                                               */
/*                                                                           */
/* Description: Print out the current time and date                          */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_date_n_time(void)
{

  time_t time_data;             /* Data from time()            */
  struct tm *time_struct;       /* Data structure holding time */
  char time_str[i_TIM_STR_LEN]; /* Buffer for time string      */


  /* Get current time */
  time_data = time(NULL);

  /* Was time obtained successfully */
  if(time_data != i_TIME_FAIL)
  {

    time_struct = localtime(&time_data);

    (void) strftime(time_str, i_TIM_STR_LEN, "%Y-%m-%d %H:%M:%S", time_struct);

    printf("%s - ", time_str);

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_time()                                                      */
/*                                                                           */
/* Description: Print out a given time                                       */
/*                                                                           */
/* Uses: The time to print                                                   */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_time(time_t time_val)
{

  time_t time_hours;
  time_t time_mins;
  time_t time_secs;


  time_hours = time_val / (time_t) i_SEC_IN_HOUR;

  time_mins = (time_val % (time_t) i_SEC_IN_HOUR) / (time_t) i_SEC_IN_MIN;

  time_secs = time_val % (time_t) i_SEC_IN_MIN;

  printf("%02ld:%02ld:%02ld", (long) time_hours, (long) time_mins,
    (long) time_secs);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_runtime()                                                   */
/*                                                                           */
/* Description: Print out the runtime of the test                            */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_runtime(void)
{

  /* Only print out if there is a valid runtime */
  if(i_runtime != i_TIME_FAIL)
  {

    if(i_runtime == 0)
    {

      printf(" run:<1sec");

    }
    else
    {

      printf(" run:");

      i_print_time(i_runtime);

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_get_runtime()                                                     */
/*                                                                           */
/* Description: Calculate the current runtime                                */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: The runtime in seconds                                           */
/*                                                                           */
/*****************************************************************************/

static time_t i_get_runtime(void)
{

  time_t time_now = 0;      /* The current time                           */
  time_t runtime = 0;       /* Length of time the test has run in seconds */


  time_now = time(NULL);

  /* Calculate runtime */
  if( (i_start_time == i_TIME_FAIL) || (time_now == i_TIME_FAIL) )
  {

    fprintf(stderr, "Failure reading time\n");

    /* Signal failure */
    runtime = i_TIME_FAIL;

  }
  else
  {

    runtime = (time_t) difftime(time_now, i_start_time);

  }

  return runtime;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_report_stats()                                                    */
/*                                                                           */
/* Description: Report the test stats                                        */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_report_stats(void)
{

  if (i_show_stats == true)
  {

    printf("\nMin return time = %ld.%06ld\n", (long) i_delta_time_min.tv_sec,
           (long) i_delta_time_min.tv_usec);

    printf("Max return time = %ld.%06ld\n", (long) i_delta_time_max.tv_sec,
           (long) i_delta_time_max.tv_usec);

    printf("Average return time = %ld.%06ld", (long) i_delta_time_av.tv_sec,
           (long) i_delta_time_av.tv_usec);

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_report_results()                                                  */
/*                                                                           */
/* Description: Report how the test went                                     */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_report_results(void)
{

  printf(" sent:");

  i_print_big_num(i_bytes_sent, i_bin_not_dec);

  printf(" errs:");

  i_print_big_num(i_num_errors, i_bin_not_dec);

  printf(" timeouts:");

  i_print_big_num(i_num_timeouts, i_bin_not_dec);

  printf(" corrupt:");

  i_print_big_num(i_num_corrupts, i_bin_not_dec);

  i_print_runtime();

  if (i_show_stats == true)
  {

    printf(" Av:%ld.%06ld", (long) i_delta_time_av.tv_sec,
           (long) i_delta_time_av.tv_usec);

  }
  
  
}


/*****************************************************************************/
/*                                                                           */
/* Name: i_show_intermediate()                                               */
/*                                                                           */
/* Description: Show intermediate test results                               */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_show_intermediate(void)
{

  time_t runtime = 0;       /* Length of time the test has run in seconds */


  /* Have intermediate results been requested? */

  if( (i_intermediate == true) || (i_i_pressed == true) )
  {

    runtime = i_get_runtime();

    /* Did we get runtime ok? */
    if(runtime != i_TIME_FAIL)
    {

      /* Are we ready to show results? */
      if( (runtime >= (i_last_results + i_intermediate_time) )
        || (i_i_pressed == true) )
      {

        i_runtime = runtime;

        /* Show the intermediate results */
        i_report_results();

        if(i_i_pressed == true)
        {

          printf("\n");

        }
        else
        {

          printf("      \r");

        }

        /* Note when we last displayed results */
        i_last_results = runtime;

        /* Reset 'i' pressed flag */
        i_i_pressed = false;

      }

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_baud()                                                      */
/*                                                                           */
/* Description: Prints out the current baud rate                             */
/*                                                                           */
/* Uses: baud_rate - The baud rate to be printed                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_baud(speed_t baud_rate)
{

  serp_baud_str_t baud_str;               /* The baud rate as a string */
  serp_baud_str_status_t baud_str_status; /* The 'get' status          */


  /* Get the string for the baud rate */
  baud_str_status = serp_get_baud_str(baud_rate, baud_str);

  if(baud_str_status != SERP_GET_BAUD_STR_FAIL)
  {

    printf("%s", (char *) baud_str);

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_print_str()                                                       */
/*                                                                           */
/* Description: Prints out the given string in a format for diagnostics.     */
/*                                                                           */
/* Uses: str_buf - The buffer for the string                                 */
/*       str_len - The string length                                         */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_print_str(unsigned char str_buf[], unsigned int str_len)
{

  unsigned int char_count = 0;          /* The loop count */


  if(str_len > i_CHARS_PER_LINE_HEX8 )
  {

    printf("\n");

  }

  /* Loop round the string */
  for(char_count = 0; char_count < str_len; char_count++)
  {

    printf("%02x", (unsigned int) str_buf[char_count]);

    if( ( (char_count + 1) % i_CHARS_PER_LINE_HEX8) == 0)
    {

      printf("\n");

    }
    else
    {

      printf(" ");

    }

  }

  printf("\n");

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_get_timeout()                                                     */
/*                                                                           */
/* Description: Calculates the timeout for the current baud rate             */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_get_timeout(void)
{

  serp_timeout_t timeout; /* The timeout */


  if (i_user_set_timeout == false)
  {

    timeout = serp_get_timeout(i_baud_rate);

    if(timeout != SERP_GET_TIMEOUT_FAIL)
    {

      i_read_timeout = timeout;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_write_serial()                                                    */
/*                                                                           */
/* Description: Write to the serial port                                     */
/*                                                                           */
/* Uses: send_buf - buffer for the bytes to send.                            */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_write_serial(unsigned char send_buf)
{

  serp_tx_buf_t tx_buf; /* Struct for transmit byte & returned status */


  /* Put the byte to transmit in the TX buffer */
  tx_buf.tx_byte = send_buf;

  /* Clear the status */
  tx_buf.tx_status = 0;

  /* Set the file descriptor */
  tx_buf.fd = i_fd;

  /* Write byte to serial port */
  serp_write_port(&tx_buf, i_diags);

  /* Did we transmit without problem */
  if( (tx_buf.tx_status & SERP_WRITE_OK) > 0)
  {

    if( (tx_buf.tx_status & SERP_WRITE_TIME_FAIL) == 0)
    {

      /* Store away the transmit time */
      i_tx_time.tv_sec = tx_buf.tx_time.tv_sec;

      i_tx_time.tv_usec = tx_buf.tx_time.tv_usec;

    }
    else
    {

      /* Indicate time read failure */
      i_tx_time.tv_sec = i_TIME_FAIL;

    }

    if(i_verbose == true)
    {

      printf("TX: %02x\n", (unsigned int) send_buf);

    }

    /* Inc the number of bytes that have been sent */
    i_bytes_sent++;

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_wait_for_write()                                                  */
/*                                                                           */
/* Description: Wait until the serial port is ready to send a byte           */
/*                                                                           */
/* Uses: send_buf - buffer for the byte to send.                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_wait_for_write(unsigned char send_buf)
{

  serp_tx_wait_status_t wait_status; /* Result of waiting for TX */


  /* Wait until we are ready to write, or timeout */
  wait_status = serp_wait_for_write(i_fd, i_DEFAULT_WRITE_TIMEOUT, i_diags);

  /* Ready to write? */
  if( (wait_status & SERP_TX_WAIT_READY) > 0)
  {

    i_write_serial(send_buf);

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_store_max_delta()                                                 */
/*                                                                           */
/* Description: Store a max delta time, if it is a new max                   */
/*                                                                           */
/* Uses: delta_time - The potential new max time                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_store_max_delta(struct timeval delta_time)
{

  /* store the max delta time */
  if (delta_time.tv_sec > i_delta_time_max.tv_sec)
  {

    i_delta_time_max.tv_sec = delta_time.tv_sec;
    i_delta_time_max.tv_usec = delta_time.tv_usec;

  }
  else
  {

    if ( (delta_time.tv_usec > i_delta_time_max.tv_usec)
       && (delta_time.tv_sec == i_delta_time_max.tv_sec) )
    {

      i_delta_time_max.tv_sec = delta_time.tv_sec;
      i_delta_time_max.tv_usec = delta_time.tv_usec;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_store_min_delta()                                                 */
/*                                                                           */
/* Description: Store a min delta time, if it is a new min                   */
/*                                                                           */
/* Uses: delta_time - The potential new min time                             */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_store_min_delta(struct timeval delta_time)
{

  if (delta_time.tv_sec < i_delta_time_min.tv_sec)
  {

    i_delta_time_min.tv_sec = delta_time.tv_sec;
    i_delta_time_min.tv_usec = delta_time.tv_usec;

  }
  else
  {

    if ( (delta_time.tv_usec < i_delta_time_min.tv_usec)
       && (delta_time.tv_sec == i_delta_time_min.tv_sec) )
    {

      i_delta_time_min.tv_sec = delta_time.tv_sec;
      i_delta_time_min.tv_usec = delta_time.tv_usec;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_store_av_delta()                                                  */
/*                                                                           */
/* Description: Calculate the average delta time                             */
/*                                                                           */
/* Uses: delta_time                                                          */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_store_av_delta(struct timeval delta_time)
{
  unsigned long long total;
  unsigned long long remainder;
  unsigned long long rollover = 0;

  /* Set up the value on the first loop */
  if(  i_bytes_sent == 1 )
  {

    i_delta_time_av.tv_sec = delta_time.tv_sec;
    i_delta_time_av.tv_usec = delta_time.tv_usec;

  }
  else
  {

    /* Calculate new average */

    /* Interger part */
    total = (i_delta_time_av.tv_sec * (i_bytes_sent - 1) ) + delta_time.tv_sec;

    i_delta_time_av.tv_sec = total / i_bytes_sent;

    /* Calc any remainder for the fractional part */
    remainder = 1000000 * (total % i_bytes_sent);

    if (remainder > 0)
    {

      rollover = remainder / i_bytes_sent;

    }

    /* Fractional part */
    total = (i_delta_time_av.tv_usec * (i_bytes_sent - 1) ) + delta_time.tv_usec;

    i_delta_time_av.tv_usec = total / i_bytes_sent;

    /* Add the remainder from the interger part */
    i_delta_time_av.tv_usec = i_delta_time_av.tv_usec + rollover;

    /* Check for overflow, to feed back to the integer part */
    if( i_delta_time_av.tv_usec >= 1000000)
    {

       i_delta_time_av.tv_usec = i_delta_time_av.tv_usec - 1000000;

       i_delta_time_av.tv_sec++;

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_read_serial()                                                     */
/*                                                                           */
/* Description: Read from the serial port                                    */
/*                                                                           */
/* Uses: sent_byte - The byte previously been sent                           */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_read_serial(unsigned char sent_byte)
{

  serp_rx_buf_t rx_buf;       /* Struct for received byte & status */

  struct timeval delta_time;  /* The time taken from tx to rx      */


  /* Clear status */
  rx_buf.rx_status = 0;

  rx_buf.fd = i_fd;

  /* Read byte from serial port */
  serp_read_port(&rx_buf, i_diags);

  /* Did all go well? */
  if( (rx_buf.rx_status & SERP_READ_OK) > 0)
  {

    /* Did a framing error occur? */
    if( (rx_buf.rx_status & SERP_READ_FRAMERR) > 0)
    {

      /* Don't report duff bytes if in quiet mode */
      if(i_quiet == false)
      {

        /* Print out current time */
        i_print_date_n_time();

        /* Print out error message */
        printf("Framing error: TX: %02x RX: %02x\n",
          (unsigned int) sent_byte, (unsigned int) rx_buf.rx_byte);

      }

      i_num_errors++;

      i_num_corrupts++;

    }
    else
    {

      /* No RX error occured */

      /* Only report byte in verbose mode */
      if(i_verbose == true)
      {

        printf("RX: %02x\n", (unsigned int) rx_buf.rx_byte);

      }

      /* Is the byte the same as the one sent */
      if(rx_buf.rx_byte != sent_byte)
      {

        /* Don't report duff bytes if in quiet mode */
        if(i_quiet == false)
        {

          /* Print out current time */
          i_print_date_n_time();

          /* Print out error message */
          printf("Corrupt byte: TX: %02x RX: %02x\n",
            (unsigned int) sent_byte, (unsigned int) rx_buf.rx_byte);

        }

        i_num_errors++;

        i_num_corrupts++;

      }
      else
      {

        /* Are times ok? */
        if( ( (rx_buf.rx_status & SERP_READ_TIME_FAIL) == 0) 
          && (i_tx_time.tv_sec != i_TIME_FAIL) )
        {

          /* Get the delta time, i.e. the time from tx to rx */
          timersub(&rx_buf.rx_time, &i_tx_time, &delta_time);

          /* Why are negatives coming through? Don't know, */
          /* but protect against them                      */
          if( (delta_time.tv_sec >= 0) && (delta_time.tv_usec >= 0) )
          {

            /* store the max delta time */
            i_store_max_delta(delta_time);

            /* store the min delta time */
            i_store_min_delta(delta_time);

            /* store the average delta time */
            i_store_av_delta(delta_time);

            if( (i_show_stats == true) && (i_verbose == true) )
            {

              printf("Char return time: %ld.%06ld\n", (long) delta_time.tv_sec,
                     (long) delta_time.tv_usec);

            }

          }

        }

      }

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_wait_for_read()                                                   */
/*                                                                           */
/* Description: Wait until a read from the serial port is ready, or timeout  */
/*                                                                           */
/* Uses: sent_byte - The byte to be sent.                                    */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_wait_for_read(unsigned char sent_byte)
{

  serp_rx_wait_status_t wait_status; /* Result of waiting for byte */


  /* Wait until we are ready to read, or timeout */
  wait_status = serp_wait_for_read(i_fd, i_read_timeout, i_diags);

  /* Choose what we do now */
  switch (wait_status)
  {

    case SERP_RX_WAIT_FAILURE: /* Did it all go wrong */

      /* Do nothing */

      break;

    case SERP_RX_WAIT_READY: /* Got something to read? */

      i_read_serial(sent_byte);

      break;

    case SERP_RX_WAIT_TIMEOUT: /* Or did we timeout */

      /* Don't report timeout if in quiet mode */
      if(i_quiet == false)
      {

        /* Print out current time */
        i_print_date_n_time();

        printf("Timeout\n");

      }

      i_num_errors++;

      i_num_timeouts++;

      break;

    default:

      /* Invalid value */
      fprintf(stderr, "Unknown value when waiting for read\n");

      break;

  } /* End switch() */

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_send_n_receive()                                                  */
/*                                                                           */
/* Description: Pick a byte, send it & wait for it to come back              */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_send_n_receive()
{

  unsigned char tx_byte;  /* The byte to transmit  */
  int flush_result;       /* Result of the flush   */


  /* Are we in random mode */
  if(i_random == true)
  {

    /* Generate random byte to send */
    tx_byte = (unsigned char) rand();

  }
  else
  {

    /* Get next byte to send */
    tx_byte = i_tx_buf[i_tx_ptr];

    /* Increment pointer to the byte to send */
    i_tx_ptr++;

    /* Wrap round the pointer to the beginning of the buffer */
    i_tx_ptr %= i_str_len;

  }

  /* Write to the port */
  i_wait_for_write(tx_byte);

  /* Read from the port */
  i_wait_for_read(tx_byte);

  /* Flush port to get rid of any bits of the last RX */
  /* Dump flush result                                */
  flush_result = serp_flush_port(i_fd, i_diags);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_check_keys()                                                      */
/*                                                                           */
/* Description: Check and process keypresses                                 */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_check_keys(void)
{

  int key_buf;  /* Buffer for keypresses */


  /* Check the keyboard */
  key_buf = getchar();

  /* Has there been a keypress? */
  if(key_buf != -1)
  {

    /* Make all chars lower case */
    key_buf = tolower(key_buf);

    /* Check what was pressed, and act on it */
    switch (key_buf)
    {

      case 'q': /* 'q' pressed, flag we want to quit */

        i_q_pressed = true;

        break;

      case 'i': /* 'i' pressed, flag we want intermediate results */

        i_i_pressed = true;

        break;

      default:

        /* Don't know what to do with that char, so bell */
        printf("\a");

        break;

    } /* End switch() */

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_pause()                                                           */
/*                                                                           */
/* Description: Wait, for a bit                                              */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/*****************************************************************************/

static int i_pause(double pause_time)
{

  struct timespec tv;        /* Structure to hold the requested time */
  int nanosleep_result = 0;  /* The return from the nanosleep call   */
  bool finished = false;     /* Have we finished sleeping yet?       */


  if( pause_time != (double) 0)
  {

    /* Sort out the seconds */
    tv.tv_sec = (time_t) pause_time;

    /* ... and then the remainder in nanoseconds. */
    tv.tv_nsec = (long) ((pause_time - tv.tv_sec) * 1e+9);

    while (finished == false)
    {

      /* Sleep for the time specified in tv. If interrupted by a      */
      /* signal, place the remaining time left to sleep back into tv. */
      nanosleep_result = nanosleep(&tv, &tv);

      /* Have we finished yet? */
      if (nanosleep_result == 0)
      {

        /* Completed the entire sleep time; all done, Let's exit. */
        finished = true;

      }
      else
      {

        /* Did an error occur? If not stay in loop */
        if (errno != EINTR)
        {

          /* An error did occur; bail out. */
          finished = true;

        }

      }

    } /* End while */

  }

  return nanosleep_result; /*  */

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_char_pause()                                                      */
/*                                                                           */
/* Description: Add a delay between chars, if requested.                     */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: A flag indicating success or failure                             */
/*                                                                           */
/*****************************************************************************/

static int i_char_pause(void)
{

  int pause_result = 0;  /* The return from the nanosleep call   */
  double time_portion;   /* A small chunk of time to delay       */
  double remaining_time; /* The time remaining                   */


  remaining_time = i_paced_time;

  while(remaining_time > 0)
  {

    if(remaining_time <= i_ESCAPE_TIME)
    {

      time_portion = remaining_time;

      remaining_time = 0;

    }
    else
    {

      time_portion = i_ESCAPE_TIME;

      remaining_time = remaining_time - i_ESCAPE_TIME;

    }

    pause_result = i_pause(time_portion);

    /* Check for keypresses */
    i_check_keys();

    /* Show intermediate results */
    i_show_intermediate();

    if(i_q_pressed == true)
    {

      /* Break out of while loop */
      remaining_time = 0;

    }

  } /* End while */
  
  return pause_result;
  
}


/*****************************************************************************/
/*                                                                           */
/* Name: i_bert_by_num()                                                     */
/*                                                                           */
/* Description: Perform a bit error rate test for a given number of bytes    */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_bert_by_num()
{

  unsigned long long bytes_sent; /* Current count of bytes sent  */

  /* Send bytes to send */
  for(bytes_sent = 0; ( (bytes_sent < i_tx_len) && (i_q_pressed == false) );
    bytes_sent++)
  {

    /* Do the sending and receiving stuff */
    i_send_n_receive(i_fd);

    /* Check for keypresses */
    i_check_keys();

    /* Show intermediate results */
    i_show_intermediate();

    /* Put in a delay for paced output */
    (void) i_char_pause();

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_bert_by_time()                                                    */
/*                                                                           */
/* Description: Perform a bit error rate test for a given time               */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_bert_by_time()
{

  time_t runtime = 0;       /* Length of time the test has run in seconds */


  /* Send bytes for given time */
  while( (runtime < i_send_time) && (i_q_pressed == false) )
  {

    /* Do the sending and receiving stuff */
    i_send_n_receive(i_fd);

    /* Check for keypresses */
    i_check_keys();

    /* Show intermediate results */
    i_show_intermediate();

    /* Check the runtime */
    runtime = i_get_runtime();

    /* Did we get runtime ok? */
    if(runtime == i_TIME_FAIL)
    {

      /* Cause breakout of program */
      runtime = i_send_time;

    }

    /* Put in a delay for paced output */
    (void) i_char_pause();

  } /* End while */

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_bert_continuous()                                                 */
/*                                                                           */
/* Description: Perform a bit error rate test continuously                   */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_bert_continuous()
{

  /* Send bytes for given time */
  while(i_q_pressed == false)
  {

    /* Do the sending and receiving stuff */
    i_send_n_receive(i_fd);

    /* Check for keypresses */
    i_check_keys();

    /* Show intermediate results */
    i_show_intermediate();

    /* Put in a delay for paced output */
    (void) i_char_pause();

  } /* End while */

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_bert()                                                            */
/*                                                                           */
/* Description: Perform a bit error rate test                                */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_bert()
{

  time_t stop_time;  /* The time the test finished */


  i_start_time = time(NULL);

  /* Select how to do the test: number, time or continuous */
  switch (i_how_test)
  {

    case i_TEST_NUM: /* Test by the number of bytes to send */

      i_bert_by_num(i_fd);

      break;

    case i_TEST_TIME: /* Test by time */

      i_bert_by_time(i_fd);

      break;

    case i_TEST_CONTINUOUS: /* Test continuously */

      i_bert_continuous(i_fd);

      break;

    default:

      /* Don't know what to do, so test by number */
      i_bert_by_num(i_fd);

      break;

  } /* End switch() */

  stop_time = time(NULL);

  /* Calculate runtime */
  if( (i_start_time != i_TIME_FAIL) && (stop_time != i_TIME_FAIL) )
  {

    i_runtime = (time_t) difftime(stop_time, i_start_time);

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_paced()                                                   */
/*                                                                           */
/* Description: Check and process the time between sending bytes. The str    */
/*              terminating char indicates secs or msecs.                    */
/*                                                                           */
/* Uses: paced_str - Pointer to a string which is the time, in secs or       */
/*                   msecs, to delay sending a byte.                         */
/*                                                                           */
/* Returns: Status indicating if the given time string is valid, or not      */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_paced(char *paced_str)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  double paced_time = 0;                 /* The time between chars          */


  /* Convert time to numeric */
  paced_time = strtod(paced_str, (char**) NULL);


  /* strtod signals an overflow and underflow error with HUGE_VAL and 0 */
  /* respectively. Also, nanopause does not like values over 2147483647 */
  /* (LONG_MAX) (Sees it as negative?) and we want 1 nanosec as the     */
  /* minimum.                                                           */
  if( (paced_time == HUGE_VAL) || (paced_time < 0.000000001) || 
    (paced_time > LONG_MAX))
  {

    /* Given string is out of range */
    fprintf(stderr, "Invalid paced time\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store paced delay time */
    i_paced_time = paced_time;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_knum_bytes()                                              */
/*                                                                           */
/* Description: Check and process the number of bytes to send in k or K      */
/*                                                                           */
/* Uses: knum_str - Pointer to a string which is the num of bytes to send    */
/*       bin_not_dec - Is the multiplier binary or decimal: 1000 or 1024     */
/*                                                                           */
/* Returns: Status indicating if the bytes to send string is valid, or not   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_knum_bytes(char *knum_str,
  bin_not_dec_t bin_not_dec)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  unsigned long knum_bytes = 0;          /* No. of k bytes as a number      */
  unsigned long kbyte;                   /* Value for 1 k - 1000 or 1024    */


  /* Select the value for 1K - 1000 or 1024 */
  if(bin_not_dec == i_BIN)
  {

    kbyte = i_KIBIBYTE;

  }
  else
  {

    kbyte = i_KILOBYTE;

  }

  /* Convert knum bytes string into an unsigned long */
  knum_bytes = strtoul(knum_str, (char**) NULL, 10);

  /* Is value invalid? */
  if( (knum_bytes >= ULONG_MAX) || (knum_bytes < 1) )
  {

    fprintf(stderr, "Invalid number of bytes to send argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid number of bytes to transmit, multiplied by 1k */
    i_tx_len = knum_bytes * kbyte;

    /* Set binary or decimal multipliers to decimal (1000) */
    i_bin_not_dec = bin_not_dec;

    /* Note the user has requested to send by number of bytes */
    i_how_test = i_TEST_NUM;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_dec_knum_bytes()                                          */
/*                                                                           */
/* Description: Check and process the number of bytes to send in k (1000)    */
/*                                                                           */
/* Uses: knum_str - Pointer to a string which is the num of bytes to send    */
/*                                                                           */
/* Returns: Status indicating if the bytes to send string is valid, or not   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_dec_knum_bytes(char *knum_str)
{

  return i_process_knum_bytes(knum_str, i_DEC);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_bin_knum_bytes()                                          */
/*                                                                           */
/* Description: Check and process the number of bytes to send in K (1024)    */
/*                                                                           */
/* Uses: knum_str - Pointer to a string which is the num of bytes to send    */
/*                                                                           */
/* Returns: Status indicating if the bytes to send string is valid, or not   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_bin_knum_bytes(char *knum_str)
{

  return i_process_knum_bytes(knum_str, i_BIN);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_num_bytes()                                               */
/*                                                                           */
/* Description: Check and process the number of bytes to send                */
/*                                                                           */
/* Uses: num_str - Pointer to a string which is the num of bytes to send     */
/*                                                                           */
/* Returns: Status indicating if the bytes to send string is valid, or not   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_num_bytes(char *num_str)
{

  arg_status_t arg_status = i_ARG_VALID;  /* Flag indicating if arg is valid */
  unsigned long num_bytes = 0;            /* No. of bytes as a number        */


  /* Convert num bytes string into an unsigned long */
  num_bytes = strtoul(num_str, (char**) NULL, 10);

  /* Is value invalid? */
  if( (num_bytes >= ULONG_MAX) || (num_bytes < 1) )
  {

    fprintf(stderr, "Invalid number of bytes to send argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid number of bytes to transmit */
    i_tx_len = (unsigned long long) num_bytes;

    /* Note the user has requested to send by number of bytes */
    i_how_test = i_TEST_NUM;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_mins()                                                    */
/*                                                                           */
/* Description: Check and process the minutes to send command line argument  */
/*                                                                           */
/* Uses: mins_str - Pointer to a string which is the number of mins to use   */
/*                                                                           */
/* Returns: Status indicating if the mins string is valid, or not            */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_mins(char *mins_str)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  time_t mins_num = 0;                   /* Minutes as a number             */


  /* Convert mins string into an long */
  mins_num = (time_t) strtol(mins_str, (char**) NULL, 10);

  /* Is value invalid? */
  if( (mins_num > ( i_TIME_MAX / i_SEC_IN_MIN) ) || (mins_num < 1) )
  {

    fprintf(stderr, "Invalid minutes argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid send time value in seconds */
    i_send_time = mins_num * (time_t) i_SEC_IN_MIN;

    /* Note the user has requested to send by time */
    i_how_test = i_TEST_TIME;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_hours()                                                   */
/*                                                                           */
/* Description: Check and process the hours to send command line argument    */
/*                                                                           */
/* Uses: hours_str - Pointer to a string which is the number of hours to use */
/*                                                                           */
/* Returns: Status indicating if the hours string is valid, or not           */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_hours(char *hours_str)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  time_t hours_num = 0;                  /* Hours as a number               */


  /* Convert hours string into an long */
  hours_num = (time_t) strtol(hours_str, (char**) NULL, 10);

  /* Is value invalid? */
  if( (hours_num > ( i_TIME_MAX / i_SEC_IN_HOUR) ) || (hours_num < 1) )
  {

    fprintf(stderr, "Invalid hours argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid send time value in seconds */
    i_send_time = hours_num * (time_t) i_SEC_IN_HOUR;

    /* Note the user has requested to send by time */
    i_how_test = i_TEST_TIME;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_intermediate()                                            */
/*                                                                           */
/* Description: Check and process the intermediate command line argument     */
/*                                                                           */
/* Uses: disp_tim_str - Pointer to a string which is the No. of secs to use  */
/*                                                                           */
/* Returns: Status indicating if the seconds string is valid,or not          */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_intermediate(char *disp_tim_str)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  time_t disp_tim_num = 0;               /* Display time as a number        */


  /* Convert display time string into an long */
  disp_tim_num = (time_t) strtol(disp_tim_str, (char**) NULL, 10);

  /* Is value invalid? */
  if( (disp_tim_num >= (time_t) LONG_MAX) || (disp_tim_num < 1) )
  {

    fprintf(stderr, "Invalid intermediate (i-) argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid display time value */
    i_intermediate_time = disp_tim_num;

    /* Note the user has requested intermediate results */
    i_intermediate = true;

    /* Quiet mode would be a good idea */
    i_quiet = true;

    /* Intermediate and verbose don't mix */
    i_verbose = false;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_timeout()                                                 */
/*                                                                           */
/* Description: Check and process the read timeout command line argument     */
/*                                                                           */
/* Uses: timeout_str - Pointer to a string which is the read timeout to use  */
/*                                                                           */
/* Returns: Status indicating if the timeout string is valid, or not         */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_timeout(char *timeout_str)
{

  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if arg is valid */
  serp_timeout_t timeout_num = 0;        /* Timeout as a number             */


  /* Convert timeout string into an unsigned long */
  timeout_num = (serp_timeout_t) strtoul(timeout_str, (char**) NULL, 10);

  /* Is value invalid? (greater than 10 secs or less than 1 microsec) */
  if( (timeout_num > SERP_TIMEOUT_MAX) || (timeout_num < 1) )
  {

    fprintf(stderr, "Invalid timeout argument\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store valid read timeout value*/
    i_read_timeout = timeout_num;

    /* Note the user has set the timeout */
    i_user_set_timeout = true;

    arg_status = i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_hex_to_byte()                                                     */
/*                                                                           */
/* Description: Convert a 2 byte string from ASCII hex to a 1 byte value     */
/*                                                                           */
/* Uses: str - Pointer to the string                                         */
/*       val - Pointer to the buffer for the return value                    */
/*                                                                           */
/* Returns: Status indicating if string is valid, or not                     */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_hex_to_byte(char *str, unsigned char *val)
{

  size_t str_len = 0;                    /* Length of given string          */
  arg_status_t arg_status = i_ARG_VALID; /* Flag indicating if str is valid */


  /* Get length of string */
  str_len = strlen(str);

  /* Check string is not too long */
  if(str_len != 2)
  {

    fprintf(stderr, "Not a hex value\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Check if both bytes are valid hex */
    if( (isxdigit(str[0]) == 0) || (isxdigit(str[1]) == 0) )
    {

      fprintf(stderr, "Not a hex value\n");

      arg_status = i_ARG_INVALID;

    }
    else
    {

      /* Convert string to value */
      *val = (unsigned char) strtol(str, (char**) NULL, 16);

      arg_status = i_ARG_VALID;

    }

  }

  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_str()                                                     */
/*                                                                           */
/* Description: Check and process the string to send                         */
/*                                                                           */
/* Uses: str - Pointer the string to send                                    */
/*                                                                           */
/* Returns: Status indicating if string is valid, or not                     */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_str(char *str)
{

  void *dummy_ptr = NULL;        /* Dummy ptr for unused returns           */
  size_t arg_len = 0;            /* Length of given string                 */
  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if the string is valid */
  unsigned int current_char = 0; /* The current char being worked on       */
  char buf[3];                   /* Buffer for the char being worked on    */
  unsigned char temp_tx_buf[i_TX_BUF_LEN];
                                 /* Temp buffer for TX string              */


  /* Get length of string */
  arg_len = strlen(str);

  /* Are there an even number of characters in the string? */
  if( (arg_len % 2) != 0 )
  {

    fprintf(stderr, "String invalid\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Loop round given string, 2 chars at a time, while valid */
    for(current_char = 0;
       ( (arg_status == i_ARG_VALID) &&
       ( (current_char * 2) < ( unsigned int) arg_len) );
       current_char++)
    {

      /* Store pair of chars in temp buffer */
      dummy_ptr = memmove(buf, &(str[(current_char * 2)]), 2);

      /* Mark string end in temp buf */
      buf[2] = (char) i_STR_TERM;

      /* Convert string to value */
      arg_status = i_hex_to_byte(buf, &(temp_tx_buf[current_char]) );

    }

    /* Store tx string and string length only if arg string was valid */
    if(arg_status == i_ARG_VALID)
    {

      dummy_ptr = memmove(i_tx_buf, temp_tx_buf, (size_t) (current_char + 1));

      i_str_len = current_char;

      /* If we've got a string, we don't want random */
      i_random = false;

    }

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_baud()                                                    */
/*                                                                           */
/* Description: Check and process the baud rate command line argument        */
/*                                                                           */
/* Uses: baud_str - Pointer to a string which is the baud rate to use        */
/*                                                                           */
/* Returns: Status indicating if the baud rate string is valid, or not       */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_baud(char *baud_str)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if baud rate is valid */
  unsigned long baud_num = 0;    /* Baud rate as a number                 */
  speed_t baud;                  /* The baud rate constant                */


  /* Convert baud rate string into an int */
  baud_num = strtoul(baud_str, (char**) NULL, 10);

  baud = serp_get_baud(baud_num);

  if(baud == SERP_GET_BAUD_FAIL)
  {

    arg_status = i_ARG_INVALID;

  }
  else
  {

    i_baud_rate = baud;

    arg_status = i_ARG_VALID;

  }

  if(arg_status == i_ARG_VALID)
  {

    /* Recalculate read timeout as baud rate has changed */
    i_get_timeout();

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_port()                                                    */
/*                                                                           */
/* Description: Check and process the serial port command line argument      */
/*                                                                           */
/* Uses: port - Pointer to a string which is the serial port to use          */
/*                                                                           */
/* Returns: Status indicating if port string is valid, or not                */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_port(char *port)
{

  void *dummy_ptr = NULL;        /* Dummy ptr for unused returns            */
  size_t arg_len = 0;            /* Length of given command line argument   */
  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if port str is valid    */


  /* Get length of port string */
  arg_len = strlen(port);

  /* Check port string is not too long */
  if(arg_len > (size_t) i_MAX_ARG_LEN)
  {

    fprintf(stderr, "Port argument too long\n");

    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Store port string */
    dummy_ptr = memmove(i_serial_port, port,(arg_len + 1));

    arg_status =  i_ARG_VALID;

  }

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_cont()                                                    */
/*                                                                           */
/* Description: Check and process the continuous command line argument       */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_cont(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_how_test = i_TEST_CONTINUOUS;

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_diag()                                                    */
/*                                                                           */
/* Description: Check and process the diagnostic command line argument       */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_diag(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_diags = true;

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_quiet()                                                   */
/*                                                                           */
/* Description: Check and process the quiet command line argument            */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_quiet(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_quiet = true;

  /* Verbose and quiet don't mix */
  i_verbose = false;

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_random()                                                  */
/*                                                                           */
/* Description: Check and process the random command line argument           */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_random(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_random = true;

  /* Return status */
  return arg_status;

}

/*****************************************************************************/
/*                                                                           */
/* Name: i_process_further()                                                 */
/*                                                                           */
/* Description: Check and process the further information command line       */
/*              argument                                                     */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_further(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_show_stats = true;

  /* Return status */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_low_latency()                                             */
/*                                                                           */
/* Description: Check and process the low latency command line argument      */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_low_latency(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_low_latency = true;

  /* Return status */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_verbose()                                                 */
/*                                                                           */
/* Description: Check and process the verbose command line argument          */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_verbose(void)
{

  arg_status_t arg_status = i_ARG_VALID;
                                 /* Flag indicating if argument is valid  */


  i_verbose = true;

  /* Intermediate and verbose or quiet don't mix */
  i_intermediate = false;

  i_quiet = false;

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_show_help()                                                       */
/*                                                                           */
/* Description: Guess what this does                                         */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_show_help(void)
{

  i_print_version();
  printf("\nUsage: serbert PORT [-cdfhlqrv] [-b BAUD] [-i SECS] [-k kBYTES]");
  printf(" [-K KBYTES]\n               [-m MINS] [-n BYTES] [-o HOURS]");
  printf(" [-p TIME] [-s STRING]\n               [-t TIMEOUT]\n\n");
  printf("Performs a serial Bit Error Rate Test (BERT) using the given port.");
  printf(" Transmits\nbytes and waits for their uncorrupted return. Press");
  printf("'q' for quit and 'i' for\nintermediate results.\n");
  printf("Options: [Defaults in square brackets]\n");
  printf(" -b - Baud rate to use: 50 - 115200           [");
  i_print_baud(i_DEFAULT_BAUD_RATE);
  printf("]\n -c - Continuous mode\n");
  printf(" -d - Diagnostic mode\n");
  printf(" -f - Further information\n");
  printf(" -h - Display this help\n");
  printf(" -i - Display intermediate results\n");
  printf(" -k - Number of bytes to send in k (* 1000)\n");
  printf(" -K - Number of bytes to send in K (* 1024)\n");
  printf(" -l - Use low latency\n");
  printf(" -m - Number of minutes to send\n");
  printf(" -n - Number of bytes to send                 [");
  i_print_big_num(i_DEFAULT_TX_SIZE, i_bin_not_dec);
  printf("]\n -o - Number of hours to send\n");
  printf(" -p - Time between bytes. 0.000000001 to 9999\n");
  printf(" -q - Quiet mode\n");
  printf(" -r - Send random bytes mode\n");
  printf(" -s - The string to send in hex               [00-FF]\n");
  printf(" -t - The read timeout to use in microseconds [%lu]\n",
    serp_get_timeout(i_DEFAULT_BAUD_RATE) );
  printf(" -v - Verbose mode\n");

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_help()                                                    */
/*                                                                           */
/* Description: Check and process the help command line argument             */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_help(void)
{

  arg_status_t arg_status;       /* Flag indicating if port str is valid    */


  i_show_help();

  /* Set invalid to break out of program */
  arg_status = i_ARG_INVALID;

  /* Return status - was the string OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_select_function()                                                 */
/*                                                                           */
/* Description: Check and select the appropriate function for an argument    */
/*                                                                           */
/* Uses:                                                                     */
/*      argc         - The number of command line arguments                  */
/*      argv         - Pointer to the command line arguments                 */
/*      arg          - Number of argument being processed                    */
/*      function_ptr - Pointer to function, which processes the argument     */
/*                                                                           */
/* Returns: Status indicating if argument is valid, or not                   */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_select_function(int argc, char *argv[], int arg,
                      i_pvf_arg_func_t function_ptr)
{

  size_t arg_len = 0;  /* Length of given command line argument */
  arg_status_t arg_status = i_ARG_VALID;
              /* Flag indicating if command line arguments are valid or not */


  /* Get length of command line argument */
  arg_len = strlen(argv[arg]);

  /* Is the parameter part of the same argument, or the following argument? */
  if(arg_len > 2)
  {

    /* Check port string is not too long, take - into account */
    if(arg_len > (size_t) (i_MAX_ARG_LEN + 2) )
    {

      fprintf(stderr, "Argument too long\n");

      arg_status = i_ARG_INVALID;

    }
    else
    {

      /* Use the function pointer to process the argument */
      /* Provide string, skipping over '-?'               */
      arg_status = (*function_ptr)(&argv[arg][2]);

    }

  }
  else
  {

    /* Is there another argument to process? */
    if(argc > (arg + 1) )
    {

      /* Get length of command line argument */
      arg_len = strlen(argv[arg + 1]);

      /* Check port string is not too long */
      if(arg_len > i_MAX_ARG_LEN)
      {

        fprintf(stderr, "Argument too long\n");

        arg_status = i_ARG_INVALID;

      }
      else
      {

        /* Use the function pointer to process the argument */
        /* Provide string, which is following argument      */
        arg_status = (*function_ptr)(argv[arg + 1]);

      }

    }
    else
    {

      fprintf(stderr, "Parameter missing\n");

      arg_status = i_ARG_INVALID;

    }

  }

  /* Return status - were the arguments OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_args_params()                                             */
/*                                                                           */
/* Description: Check and process command line arguments with parameters     */
/*                                                                           */
/* Uses:                                                                     */
/*      argc - The number of command line arguments                          */
/*      argv - Pointer to the command line arguments                         */
/*      start_arg - No. of next argument to process                          */
/*                                                                           */
/* Valid command line arguments:                                             */
/*   -b Baud rate to use                                                     */
/*   -c Continuous mode                                                      */
/*   -d Diagnostic mode                                                      */
/*   -f Display further information                                          */
/*   -h Display help text                                                    */
/*   -i Display intermediate results                                         */
/*   -k Number of bytes to send in k (1000)                                  */
/*   -K Number of bytes to send in K (1024)                                  */
/*   -l Use low latency                                                      */
/*   -m Number of minutes to send                                            */
/*   -n Number of bytes to send                                              */
/*   -o Number of hours to send                                              */
/*   -p Paced output                                                         */
/*   -q Quiet mode                                                           */
/*   -r Random mode                                                          */
/*   -s The string to send                                                   */
/*   -t The read timeout to use                                              */
/*   -v Verbose mode                                                         */
/*                                                                           */
/* Returns: Status indicating if arguments are valid, or not                 */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_args_params(int argc, char *argv[],
  int start_arg)
{

  arg_status_t arg_status = i_ARG_VALID;
              /* Flag indicating if command line arguments are valid or not */
  int arg;                        /* The number of the current argument     */
  size_t arg_len = 0;             /* Length of given command line argument  */
  int current_char = 0;           /* Pointer to the current char to process */
  int current_arg = 0;            /* The current arg in list                */
  bool get_out = false;           /* Get out of multiple args together loop */
  bool got_match = false;         /* Found a match for the arg              */

  struct i_argument_t arg_list[] =
  {
  /*  arg  function                  parameters */
    { 'b', i_process_baud,           1 },
    { 'c', i_process_cont,           0 },
    { 'd', i_process_diag,           0 },
    { 'f', i_process_further,        0 },
    { 'h', i_process_help,           0 },
    { 'i', i_process_intermediate,   1 },
    { 'k', i_process_dec_knum_bytes, 1 },
    { 'K', i_process_bin_knum_bytes, 1 },
    { 'l', i_process_low_latency,    0 },
    { 'm', i_process_mins,           1 },
    { 'n', i_process_num_bytes,      1 },
    { 'o', i_process_hours,          1 },
    { 'p', i_process_paced,          1 },
    { 'q', i_process_quiet,          0 },
    { 'r', i_process_random,         0 },
    { 's', i_process_str,            1 },
    { 't', i_process_timeout,        1 },
    { 'v', i_process_verbose,        0 },
    { '0',  NULL,                    0 }
  };

  /* Point to the first argument to process */
  arg = start_arg;

  /* Process each argument in turn */
  while(arg < argc && arg_status == i_ARG_VALID)
  {

    /* Check if it is marked as an argument */
    if(argv[arg][0] == '-')
    {

      /* Get the length of the argument */
      arg_len = strlen(argv[arg]);

      /* Set the pointer to after the '-' */
      current_char = 1;

      get_out = false;

      /* Loop round arguments grouped together */
      while( (current_char < (int) arg_len) && (arg_status == i_ARG_VALID)
        && (get_out == false) )
      {

        got_match = false;

        /* Look for the arg in the list */
        for(current_arg = 0; ( (arg_list[current_arg].function != NULL)
          && (got_match == false) ); current_arg++)
        {

          /* Have we found the arg in the list? */
          if(argv[arg][current_char] == arg_list[current_arg].arg)
          {

            got_match = true;

            /* Process the arg, depending on the No. of options it requires */
            switch (arg_list[current_arg].options)
            {

              case 0 : /* No options required */

                arg_status = arg_list[current_arg].function();

                break;

              case 1 : /* One option required */

                arg_status = i_select_function(argc, argv, arg,
                  arg_list[current_arg].function);

                get_out = true;

              break;

            } /* End switch */

          } /* End if */

        } /* End for */

        current_char++;

      } /* End while*/

    } /* End if */

    arg++;

  } /* End while */

  /* Return status - were the arguments OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_compulse_args()                                           */
/*                                                                           */
/* Description: Check and process compulsory command line arguments          */
/*                                                                           */
/* Uses:                                                                     */
/*      argv  - Pointer to the command line arguments                        */
/*                                                                           */
/* Valid command line arguments:                                             */
/*      Serial port to use                                                   */
/*                                                                           */
/* Returns: Status indicating if arguments are valid, or not                 */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_compulse_args(char *argv[])
{

  arg_status_t arg_status = i_ARG_VALID;
              /* Flag indicating if command line arguments are valid or not */
  int arg = 1;
              /* Count of current command line argument being processed     */


  /* Was a -h help requested */
  if( (argv[arg][0] == '-') && (argv[arg][1] == 'h') )
  {

    i_show_help();

    /* Set invalid to break out of program */
    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Process compulsory arguments */
    arg_status = i_process_port(argv[arg]);

  }

  /* Return status - were the arguments OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_process_arguments()                                               */
/*                                                                           */
/* Description: Check and process the command line arguments                 */
/*                                                                           */
/* Uses:                                                                     */
/*      argc  - The number of command line arguments                         */
/*      argv  - Pointer to the command line arguments                        */
/*                                                                           */
/* Returns: Status indicating if arguments are valid, or not                 */
/*                                                                           */
/*****************************************************************************/

static arg_status_t i_process_arguments(int argc, char *argv[])
{

  arg_status_t arg_status = i_ARG_VALID;
              /* Flag indicating if command line arguments are valid or not */
  int arg = 1;
              /* Count of current command line argument being processed     */


  /* Do we have at least 1 argument */
  if(argc < 2)
  {

    i_show_help();

    /* Set invalid to break out of program */
    arg_status = i_ARG_INVALID;

  }
  else
  {

    /* Process compulsory arguments */
    arg_status = i_process_compulse_args(argv);

    arg += i_MAX_COMP_ARGS;

    if(arg_status == i_ARG_VALID)
    {

      /* Process optional arguments with no parameters */

      /* Process optional arguments with parameters */
      arg_status = i_process_args_params(argc, argv, arg);

    }

  }

  /* Return status - were the arguments OK, or not */
  return arg_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_set_port_config()                                                 */
/*                                                                           */
/* Description: Set the configuration of the port                            */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: The status of the configuration, success or failure              */
/*                                                                           */
/*****************************************************************************/

static int i_set_port_config()
{

  int config_status = SERP_PORT_SUCCESS;
                                  /* Status of configuring a port        */
  int flush_status = SERP_PORT_SUCCESS;
                                  /* Status of flushing the port         */
  int result_status = SERP_PORT_SUCCESS;
                                  /* Status of everything                */
  struct serp_port_setup_t setup; /* Structure for setup data and status */


  /* Set the file descriptor */
  setup.fd = i_fd;

  /* Set the baud rate */
  setup.speed = i_baud_rate;

  /* Set the number of data bits */
  setup.data_bits = i_DEFAULT_DATA_BITS;

  /* Set the parity */
  setup.parity = i_DEFAULT_PARITY;

  /* Set the number of stop bits */
  setup.stop_bits = i_DEFAULT_STOP_BITS;

  /* Set the latency */
  setup.low_latency = i_low_latency;

  /* Configure the port */
  config_status = serp_set_port_config(&setup, i_diags);

  /* Flush the serial port */
  flush_status = serp_flush_port(i_fd, i_diags);

  if( (config_status == SERP_PORT_FAILURE) || 
    (flush_status == SERP_PORT_FAILURE) )
  {

    result_status = SERP_PORT_FAILURE;

  }

  return result_status;

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_report_settings()                                                 */
/*                                                                           */
/* Description: Report the currently selected settings                       */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_report_settings(void)
{

  /* Only report settings if we are in diagnostic mode */
  if(i_diags == true)
  {

    /* Print out the software version number*/
    i_print_version();

    printf("\n");

    /* Serial port being used */
    printf("Port: %s\n", i_serial_port);

    /* The test string being used */
    printf("String (Hex): ");

    if(i_random == true)
    {

      printf("Random\n");

    }
    else
    {

      i_print_str(i_tx_buf, i_str_len);

    }

    /* Baud rate being used */
    printf("Baud rate: ");

    i_print_baud(i_baud_rate);

    printf("\n");

    /* Read timeout being used */
    printf("Read timeout: %lu microsecs\n", i_read_timeout);

    /* Binary or decimal multiplier (1024 or 1000) */
    if(i_bin_not_dec == i_BIN)
    {

      printf("Number multiplier: Binary (1024)\n");

    }
    else
    {

      printf("Number multiplier: Decimal (1000)\n");

    }

    switch (i_how_test)
    {

      case i_TEST_NUM: /* Test by the number of bytes to send */

        /* No. of bytes to send */
        printf("Bytes to send: ");

        i_print_big_num(i_tx_len, i_bin_not_dec);

        printf("\n");

        break;

      case i_TEST_TIME: /* Test by time */

        printf("Time to send: ");

        i_print_time(i_send_time);

        printf("\n");


        break;

      case i_TEST_CONTINUOUS: /* Test continuously */

        printf("Sending continuously\n");

        break;

      default:

        /* Don't know what to do, so test by number */
        printf("Don't know how to test\n");

        break;

    } /* End switch() */

    printf("Pause between test bytes: %.9f secs\n", i_paced_time);

    printf("Low Latency is ");

    if(i_low_latency == true)
    {

      printf("on\n");

    }
    else
    {

      printf("off\n");

    }

    printf("\n");

  } /* End i_diags if */

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_restore_console()                                                 */
/*                                                                           */
/* Description: Restore the console settings                                 */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_restore_console(void)
{

  /* Restore the options for the port */
  (void) tcsetattr(0, TCSANOW, &i_saved_options);

  /* Restore stdin settings */
  (void) fcntl(0, F_SETFL, i_saved_flags);

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_initialise_console()                                              */
/*                                                                           */
/* Description: Initialise the console settings                              */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_initialise_console(void)
{

  int getattr_return = 0;          /* Returned by get attributes     */
  int tcsetattr_return = 0;        /* Returned by set attributes     */
  struct termios options;          /* Structure for the port options */
  int fcntl_return;                /* Return from fcntl              */


  /* Save current flags for stdin */
  i_saved_flags = fcntl(0, F_GETFL);

  if(i_saved_flags == i_STDIN_FAIL)
  {

    /* Error opening stdin */
    perror("fcntl");

  }

  /* Set required flags for stdin: non-blocking on getchar */
  fcntl_return = fcntl(0, F_SETFL, O_NONBLOCK);

  if(fcntl_return == i_STDIN_FAIL)
  {

    /* Error opening stdin */
    perror("fcntl");

  }

  /* Save the current options for stdin */
  getattr_return = tcgetattr(0, &i_saved_options);

  /* Was tcgetattr successful? */
  if(getattr_return == i_STDIN_FAIL)
  {

    fprintf(stderr, "tcgetattr failed\n");

  }
  else
  {

    /* Get the current options for the port */
    getattr_return = tcgetattr(0, &options);

    /* Was tcgetattr successful? */
    if(getattr_return == i_STDIN_FAIL)
    {

      fprintf(stderr, "tcgetattr failed\n");

    }
    else
    {

      /* Make sure we don't have to wait for a CR */
      options.c_lflag &= ~(ECHO|ICANON);

      /* Set our chosen stdin attributes */
      tcsetattr_return = tcsetattr(0, TCSANOW, &options);

    }

  }

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_restore_settings()                                                */
/*                                                                           */
/* Description: Restore system parameters                                    */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_restore_settings(void)
{

  i_restore_console();

}


/*****************************************************************************/
/*                                                                           */
/* Name: i_initialise_settings()                                             */
/*                                                                           */
/* Description: Initialise program parameters                                */
/*                                                                           */
/* Uses: void                                                                */
/*                                                                           */
/* Returns: void                                                             */
/*                                                                           */
/*****************************************************************************/

static void i_initialise_settings(void)
{

  unsigned int current_byte = 0;   /* Current byte count             */
  unsigned int seed;               /* Seed for random byte generator */


  /* Set default baud rate */
  i_baud_rate = i_DEFAULT_BAUD_RATE;

  /* Set the 'user has set the timeout'. This must be initialised before the */
  /* timeout is calculated                                                   */
  i_user_set_timeout = false;

  /* Set read timeout. This must be calculated after the baud rate is set */
  i_get_timeout();

  /* Reset the time the test started */
  i_start_time = 0;

  /* Reset the time the test took */
  i_runtime = 0;

  /* Reset the required time to send */
  i_send_time = 0;

  /* Reset the number of bytes sent */
  i_bytes_sent = 0;

  /* Reset the number of errors */
  i_num_errors = 0;

  /* Reset the number of timeout errors */
  i_num_timeouts = 0;

  /* Reset the number of corrupt byte errors */
  i_num_corrupts = 0;

  /* Fill the TX buffer with bytes - 0 to 255 */
  for(current_byte = 0; current_byte < i_NUM_CHARS; current_byte++)
  {

    i_tx_buf[current_byte] = (unsigned char) current_byte;

  }

  /* Store the string length */
  i_str_len = i_NUM_CHARS;

  /* Initialise pointer to the byte to be sent */
  i_tx_ptr = 0;

  /* Store the number of bytes to send */
  i_tx_len = i_DEFAULT_TX_SIZE;

  /* The random function used to generate random bytes      */
  /* always produces the same sequence of numbers, as it    */
  /* uses the same seed (1) as a starting point. Providing  */
  /* a psudo-random seed randomises the start. The time()   */
  /* function is used to provide this psudo-random number.  */

  /* Generate a sort of random seed */
  seed = (unsigned int) time(NULL);

  /* Randomise start of random function */
  srand(seed);

  /* Verbose mode */
  i_verbose = false;

  /* Verbose mode */
  i_quiet = false;

  /* Send random bytes mode */
  i_random = false;

  /* Report intermediate results */
  i_intermediate = false;

  /* Show stats at end of run */
  i_show_stats = false;

  /* No low latency for you */
  i_low_latency = false;

  /* Binary or decimal multipliers used (1024 or 1000) */
  i_bin_not_dec = i_DEFAULT_BIN_NOT_DEC;

  /* Diagnostic mode */
  i_diags = false;

  /* How to test */
  i_how_test = i_TEST_NUM;

  /* Flag indicating 'q' has been pressed */
  i_q_pressed = false;

  /* Flag indicating 'i' has been pressed */
  i_i_pressed = false;

  /* Time to pause between chars */
  i_paced_time = 0;

  /* The max byte turnround time     */
  i_delta_time_max.tv_sec  = 0;
  i_delta_time_max.tv_usec = 0;

  /* The min byte turnround time     */
  i_delta_time_min.tv_sec   = 999999;
  i_delta_time_min.tv_usec  = 999999;

  /* The average byte turnround time */
  i_delta_time_av.tv_sec   = 0;
  i_delta_time_av.tv_usec  = 0;

  i_initialise_console();

}


/*****************************************************************************/
/*      EXTERNAL VARIABLE DEFINITIONS                                        */
/*****************************************************************************/


/*****************************************************************************/
/*      EXTERNAL FUNCTION DEFINITIONS                                        */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Name: main()                                                             */
/*                                                                           */
/*  Description: main function for serbert                                   */
/*                                                                           */
/*  Internal functions used: i_initialise_settings()                         */
/*                           i_process_arguments()                           */
/*                           i_report_settings()                             */
/*                           i_set_port_config()                             */
/*                           i_kick_off_tasks()                              */
/*                           i_report_results()                              */
/*                           i_restore_settings()                            */
/*                                                                           */
/*  Internal variables used: i_serial_port, i_diags                          */
/*                                                                           */
/*  Parameters:                                                              */
/*    Name            Type         Comments                                  */
/*    -----------------------------------------------------------------      */
/*    argc            int          The number of command line arguments      */
/*    argv            *char        Pointer to the command line arguments     */
/*                                                                           */
/*  See i_process_arguments() for valid command line arguments               */
/*                                                                           */
/*  Returns: The exit status                                                 */
/*                                                                           */
/*****************************************************************************/

extern int main(int argc, char *argv[])
{

#ifdef SERBERT_LOCK
  serp_lock_status_t lock_status; /* The status of the lock                  */
#endif /* SERBERT_LOCK */
  int configure_status;           /* Status of configuring a port            */
  int save_configure_status;      /* Status of the save configuration        */
  int exit_status = i_EXIT_OK;    /* Status to be returned                   */
  arg_status_t arg_status = i_ARG_VALID;
               /* Flag indicating if command line arguments are valid or not */


  /* Set the default settings */
  i_initialise_settings();

  /* Check and process the command line arguments */
  arg_status = i_process_arguments(argc, argv);

  if(arg_status == i_ARG_VALID)
  {

    i_report_settings();


#ifdef SERBERT_LOCK

    /* Lock the serial port */
    lock_status = serp_lock_port(i_serial_port);

    if(lock_status == SERP_LOCK_OK)
    {

#endif /* SERBERT_LOCK */


      /* Open the serial port */
      i_fd = serp_open_port(i_serial_port, i_diags);

      /* Was port opened successfully? */
      if(i_fd != SERP_PORT_FAILURE)
      {

        /* Save all the port settings */
        save_configure_status = serp_save_port_state(i_fd, i_diags);

        /* Config the serial port */
        configure_status = i_set_port_config(i_fd);

        /* Send only if the port was configured successfully */
        if( (configure_status != SERP_PORT_FAILURE) &&
          (save_configure_status != SERP_PORT_FAILURE) )
        {

          /* Do the bert thing */
          i_bert();

          /* Report how we got on */
          i_report_results();

          i_report_stats();

          printf("\n");

        } /* End of config if */
        else
        {

          /* Flag error */
          exit_status = i_EXIT_FAULT;

        }

        /* Restore all the port settings */
        serp_restore_port_state(i_fd, i_diags);

        /* Close the serial port */
        serp_close_port(i_fd, i_diags);

      }  /* End of open if */
      else
      {

        /* Flag error */
        exit_status = i_EXIT_FAULT;

      }

#ifdef SERBERT_LOCK

      /* Unlock the serial port */
      serp_unlock_port(i_serial_port);

    }  /* End of lock if */
    else
    {

      /* Flag error, unable to lock port */
      exit_status = i_EXIT_FAULT;

    }

#endif /* SERBERT_LOCK */

  } /* End of arg if */
  else
  {

    /* Flag error */
    exit_status = i_EXIT_FAULT;

  }

  /* Restore system stuff */
  i_restore_settings();

  /* Return 0 if happy, 1 if a fault occured */
  return exit_status;

}
