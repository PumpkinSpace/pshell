/*******************************************************************************
 *
 * Copyright (c) 2009, Ron Iovine, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Ron Iovine nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Ron Iovine ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL Ron Iovine BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <PshellServer.h>

/*
 * This is an example demo program that uses all the basic features of the
 * PSHELL library, this program can be run as either a UDP, TCP,  UNIX, or
 * local server based on the command line options.  If it is run as a UDP
 * or UNIX based server, you must use the provided stand-alone UDP client
 * program 'pshell' to connect to it, if using the TCP based server you must
 * use a standard  'telnet' client, if it is run as a local server, user
 * command line input is solicited directly from this program, no external
 * client is needed.
 */

/*
 * PSHELL user callback functions, the interface is identical to the "main"
 * in C, with the argc being the argument count (excluding the actual
 * command itself), and argv[] being the argument list (again, excluding
 * the command).
 *
 * Use the special 'pshell_printf' function call to display data back to the
 * remote client.  The interface to this function is exactly the same as
 * the standard 'printf' function.
 */
int hello(int argc, char *argv[])
{
  pshell_printf("hello command dispatched:\n");
  /* dump out our args */
  for (int i = 0; i < argc; i++)
  {
    pshell_printf("  argv[%d]: '%s'\n", i, argv[i]);
  }
  return (PSHELL_COMMAND_SUCCESS);
}

int world(int argc, char *argv[])
{
  pshell_printf("world command dispatched:\n");
  return (PSHELL_COMMAND_SUCCESS);
}

/*
 * this command shows an example client keep alive,
 * the PSHELL UDP client has a default 5 second timeout, 
 * if a command will be known to take longer than 5 
 * seconds, it must give some kind of  output back to 
 * the client, this shows the two helper functions 
 * created the assist in this, the TCP client does not
 * need a keep alive since the TCP protocol itself
 * handles that
 */
int keepAlive(int argc, char *argv[])
{
  if (pshell_isEqual(argv[0], "dots"))
  {
    pshell_printf("marching dots keep alive:\n");
    for (unsigned i = 0; i < 10; i++)
    {
      pshell_march(".");
      sleep(1);
    }
  }
  else if (pshell_isEqual(argv[0], "bang"))
  {
    pshell_printf("marching 'bang' keep alive:\n");
    for (unsigned i = 0; i < 10; i++)
    {
      pshell_march("!");
      sleep(1);
    }
  }
  else if (pshell_isEqual(argv[0], "pound"))
  {
    pshell_printf("marching pound keep alive:\n");
    for (unsigned i = 0; i < 10; i++)
    {
      pshell_march("#");
      sleep(1);
    }
  }
  else if (pshell_isEqual(argv[0], "wheel"))
  {
    pshell_printf("spinning wheel keep alive:\n");
    for (unsigned i = 0; i < 10; i++)
    {
      /* string is optional, use NULL to omit */
      pshell_wheel("optional string: ");
      sleep(1);
    }
  }
  else
  {
    pshell_printf("ERROR: Invalid input\n");
    pshell_showUsage();
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  pshell_printf("\n");
  return (PSHELL_COMMAND_SUCCESS);
}

/*
 * this command shows matching the passed command arguments based on
 * substring matching rather than matching on the complete exact string,
 * the minimum number of characters that must be matched is the last
 * argument to the pshell_isSubString function, this must be the minimum
 * number of characters necessary to uniquely identify the argument from
 * the complete argument list
 *
 * NOTE: This technique could have been used in the previous example for the
 *       "wheel" and "dots" arguments to provide for wildcarding of those
 *        arguments.  In the above example, as written, the entire string of
 *        "dots" or "wheel" must be enter to be accepted.
 */

int wildcardMatch(int argc, char *argv[])
{
  if (pshell_isHelp())
  {
    pshell_printf("\n");
    pshell_showUsage();
    pshell_printf("\n");
    pshell_printf("  where valid <args> are:\n");
    pshell_printf("\n");
    pshell_printf("    on\n");
    pshell_printf("    of*f\n");
    pshell_printf("    a*ll\n");
    pshell_printf("    sy*mbols\n");
    pshell_printf("    se*ttings\n");
    pshell_printf("    d*efault\n");
    pshell_printf("\n");
    return (PSHELL_COMMAND_USAGE_REQUESTED);
  }
  else if (pshell_isSubString(argv[0], "on", 2))
  {
    pshell_printf("argv 'on' match\n");
  }
  else if (pshell_isSubString(argv[0], "off", 2))
  {
    pshell_printf("argv 'off' match\n");
  }
  else if (pshell_isSubString(argv[0], "all", 1))
  {
    pshell_printf("argv 'all' match\n");
  }
  else if (pshell_isSubString(argv[0], "symbols", 2))
  {
    pshell_printf("argv 'symbols' match\n");
  }
  else if (pshell_isSubString(argv[0], "settings", 2))
  {
    pshell_printf("argv 'settings' match\n");
  }
  else if (pshell_isSubString(argv[0], "default", 1))
  {
    pshell_printf("argv 'default' match\n");
  }
  else
  {
    pshell_printf("\n");
    pshell_showUsage();
    pshell_printf("\n");
    pshell_printf("  where valid <args> are:\n");
    pshell_printf("\n");
    pshell_printf("    on\n");
    pshell_printf("    of*f\n");
    pshell_printf("    a*ll\n");
    pshell_printf("    sy*mbols\n");
    pshell_printf("    se*ttings\n");
    pshell_printf("    d*efault\n");
    pshell_printf("\n");
    return (PSHELL_COMMAND_USAGE_REQUESTED);
  }
  return (PSHELL_COMMAND_SUCCESS);
}

/*
 * if a command is registered with the "showUsage" flag set to "false"
 * the PshellServer will invoke the command when the user types a "?" or 
 * "-h" rather than automatically giving the registered usage, the callback
 * command can then see if the user asked for help (i.e. typed a "?" or 
 * "-h") by calling pshell_isHelp, the user can then display the standard
 * registered usage with the pshell_showUsage call and then give some
 * optional enhanced usage with the pshell_printf call
 */
int enhancedUsage(int argc, char *argv[])
{

  /* see if the user asked for help */
  if (pshell_isHelp())
  {
    /* show standard usage */
    pshell_showUsage();
    /* give some enhanced usage */
    pshell_printf("Enhanced usage here...\n");
    return (PSHELL_COMMAND_USAGE_REQUESTED);
  }
  else
  {
    /* do normal function processing */
    pshell_printf("enhancedUsage command dispatched:\n");
    for (int i = 0; i < argc; i++)
    {
      pshell_printf("  argv[%d]: '%s'\n", i, argv[i]);
    }
  }
  return (PSHELL_COMMAND_SUCCESS);
}

/*
 * this function demonstrates the various helper functions that assist
 * in the interpretation and conversion of command line arguments
 */
int formatChecking(int argc, char *argv[])
{

  pshell_printf("formatChecking command dispatched:\n");

  if (pshell_isDec(argv[0]))
  {
    pshell_printf("Decimal arg: %d entered\n", pshell_getUnsigned(argv[0]));
  }
  else if (pshell_isHex(argv[0]))
  {
    pshell_printf("Hex arg: 0x%x entered\n", pshell_getUnsigned(argv[0]));
  }
  else if (pshell_isAlpha(argv[0]))
  {
    pshell_isEqual(argv[0], "myarg") ?
    pshell_printf("Alphabetic arg: '%s' equal to 'myarg'\n", argv[0]) :
    pshell_printf("Alphabetic arg: '%s' not equal to 'myarg'\n", argv[0]);
  }
  else if (pshell_isAlphaNumeric(argv[0]))
  {
    pshell_isEqual(argv[0], "myarg1") ?
    pshell_printf("Alpha numeric arg: '%s' equal to 'myarg1'\n", argv[0]) :
    pshell_printf("Alpha numeric arg: '%s' not equal to 'myarg1'\n", argv[0]);
  }
  else if (pshell_isFloat(argv[0]))
  {
    pshell_printf("Float arg: %.2f entered\n", pshell_getFloat(argv[0]));
  }
  else
  {
    pshell_printf("Unknown arg format: '%s'\n", argv[0]);
    return (PSHELL_COMMAND_INVALID_ARG_FORMAT);
  }
  return (PSHELL_COMMAND_SUCCESS);
}

/* function to show advanced command line parsing using the pshell_tokenize function */

#define MAX_YEAR   3000
#define MAX_MONTH  12
#define MAX_DAY    31
#define MAX_HOUR   23
#define MAX_MINUTE 59
#define MAX_SECOND 59

int advancedParsing(int argc, char *argv[])
{
  PshellTokens *timestamp = pshell_tokenize(argv[0], ":");

  if (timestamp->numTokens != 6)
  {
    pshell_printf("ERROR: Improper timestamp format!!\n");
    pshell_showUsage();
    return (PSHELL_COMMAND_INVALID_ARG_FORMAT);
  }
  else if (!pshell_isDec(timestamp->tokens[0]) ||
           (pshell_getInt(timestamp->tokens[0]) > MAX_YEAR))
  {
    pshell_printf("ERROR: Invalid year: %s, must be numeric value <= %d\n",
                  timestamp->tokens[0],
                  MAX_YEAR);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else if (!pshell_isDec(timestamp->tokens[1]) ||
           (pshell_getInt(timestamp->tokens[1]) > MAX_MONTH))
  {
    pshell_printf("ERROR: Invalid month: %s, must be numeric value <= %d\n",
                  timestamp->tokens[1],
                  MAX_MONTH);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else if (!pshell_isDec(timestamp->tokens[2]) ||
           (pshell_getInt(timestamp->tokens[2]) > MAX_DAY))
  {
    pshell_printf("ERROR: Invalid day: %s, must be numeric value <= %d\n",
                  timestamp->tokens[2],
                  MAX_DAY);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else if (!pshell_isDec(timestamp->tokens[3]) ||
           (pshell_getInt(timestamp->tokens[3]) > MAX_HOUR))
  {
    pshell_printf("ERROR: Invalid hour: %s, must be numeric value <= %d\n",
                  timestamp->tokens[3],
                  MAX_HOUR);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else if (!pshell_isDec(timestamp->tokens[4]) ||
           (pshell_getInt(timestamp->tokens[4]) > MAX_MINUTE))
  {
    pshell_printf("ERROR: Invalid minute: %s, must be numeric value <= %d\n",
                  timestamp->tokens[4],
                  MAX_MINUTE);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else if (!pshell_isDec(timestamp->tokens[5]) ||
           (pshell_getInt(timestamp->tokens[5]) > MAX_SECOND))
  {
    pshell_printf("ERROR: Invalid second: %s, must be numeric value <= %d\n",
                  timestamp->tokens[5],
                  MAX_SECOND);
    return (PSHELL_COMMAND_INVALID_ARG_VALUE);
  }
  else
  {
    pshell_printf("Year   : %s\n", timestamp->tokens[0]);
    pshell_printf("Month  : %s\n", timestamp->tokens[1]);
    pshell_printf("Day    : %s\n", timestamp->tokens[2]);
    pshell_printf("Hour   : %s\n", timestamp->tokens[3]);
    pshell_printf("Minute : %s\n", timestamp->tokens[4]);
    pshell_printf("Second : %s\n", timestamp->tokens[5]);
  }
  return (PSHELL_COMMAND_SUCCESS);
}

void showUsage(void)
{
  printf("\n");
  printf("Usage: pshellServerDemo -udp | -unix | -tcp | -local\n");
  printf("\n");
  printf("  where:\n");
  printf("    -udp   - Multi-session UDP server\n");
  printf("    -unix  - Multi-session UNIX domain server\n");
  printf("    -tcp   - Single session TCP server\n");
  printf("    -local - Local command dispatching server\n");
  printf("\n");
}

/*
 * setup our port number, this is the default port number used
 * if our serverName is not found in the pshell.servers file
 */
#define PSHELL_DEMO_PORT 6001

int main(int argc, char *argv[])
{

  PshellServerType serverType;

  /* validate our command line arguments and get the server type */
  if (argc == 2)
  {
    if (strcmp(argv[1], "-udp") == 0)
    {
      serverType = PSHELL_UDP_SERVER;
    }
    else if (strcmp(argv[1], "-tcp") == 0)
    {
      serverType = PSHELL_TCP_SERVER;
    }
    else if (strcmp(argv[1], "-local") == 0)
    {
      serverType = PSHELL_LOCAL_SERVER;
    }
    else if (strcmp(argv[1], "-unix") == 0)
    {
      serverType = PSHELL_UNIX_SERVER;
    }
    else
    {
      showUsage();
      return (0);
    }
  }
  else
  {
    showUsage();
    return (0);
  }

  /*
   * Register all our pshell callback commands
   *
   * NOTE: Command names consist of one keyword only
   */

  pshell_addCommand(hello,                        /* function */
                    "hello",                      /* command */
                    "hello command description",  /* description */
                    "[<arg1> ... <arg20>]",       /* usage */
                    0,                            /* minArgs */
                    20,                           /* maxArgs */
                    true);                        /* showUsage on "?" */

  pshell_addCommand(world,                        /* function */
                    "world",                      /* command */
                    "world command description",  /* description */
                    NULL,                         /* usage */
                    0,                            /* minArgs */
                    0,                            /* maxArgs */
                    true);                        /* showUsage on "?" */

  if ((serverType == PSHELL_UDP_SERVER) || (serverType == PSHELL_UNIX_SERVER))
  {
    pshell_addCommand(keepAlive,                            /* function */
                      "keepAlive",                          /* command */
                      "command to show client keep-alive",  /* description */
                      "dots | bang | pound | wheel",        /* usage */
                      1,                                    /* minArgs */
                      1,                                    /* maxArgs */
                      true);                                /* showUsage on "?" */
  }

  pshell_addCommand(wildcardMatch,                            /* function */
                    "wildcardMatch",                          /* command */
                    "command that does a wildcard matching",  /* description */
                    "<arg>",                                  /* usage */
                    1,                                        /* minArgs */
                    1,                                        /* maxArgs */
                    false);                                   /* showUsage on "?" */

  pshell_addCommand(enhancedUsage,                   /* function */
                    "enhancedUsage",                 /* command */
                    "command with enhanced usage",   /* description */
                    "<arg1>",                        /* usage */
                    1,                               /* minArgs */
                    1,                               /* maxArgs */
                    false);                          /* showUsage on "?" */

  pshell_addCommand(formatChecking,                       /* function */
                    "formatChecking",                     /* command */
                    "command with arg format checking",   /* description */
                    "<arg1>",                             /* usage */
                    1,                                    /* minArgs */
                    1,                                    /* maxArgs */
                    true);                                /* showUsage on "?" */

  pshell_addCommand(advancedParsing,                                /* function */
                    "advancedParsing",                              /* command */
                    "command with advanced command line parsing",   /* description */
                    "<yyyy>:<mm>:<dd>:<hh>:<mm>:<ss>",              /* usage */
                    1,                                              /* minArgs */
                    1,                                              /* maxArgs */
                    true);                                          /* showUsage on "?" */
                  
  /*
   * example of issuing an pshell command from within a program, this can be done before
   * or after the server is started, as long as the command being called is regstered
   */
   
  pshell_runCommand("hello");

  /*
   * Now start our PSHELL server with the pshell_startServer function call.
   *
   * The 1st argument is our serverName (i.e. "pshellServerDemo").
   *
   * The 2nd argument specifies the type of PSHELL server, the three valid values are:
   * 
   *   PSHELL_UDP_SERVER   - Server runs as a multi-session UDP based server.  This requires
   *                         the special stand-alone command line UDP/UNIX client program
   *                         'pshell'.  This server has no timeout for idle client sessions.
   *                         It can be also becontrolled programatically via an external
   *                         program runningthe PshellControl.h API.
   *   PSHELL_UNIX_SERVER  - Server runs as a multi-session UNIX based server.  This requires
   *                         the special stand-alone command line UDP/UNIX client program
   *                         'pshell'.  This server has no timeout for idle client sessions.
   *                         It can be also becontrolled programatically via an external
   *                         program runningthe PshellControl.h API.
   *   PSHELL_TCP_SERVER   - Server runs as a single session TCP based server with a 10 minute
   *                         idle client session timeout.  The TCP server can be connected to
   *                         using a standard 'telnet' based client.
   *   PSHELL_LOCAL_SERVER - Server solicits it's own command input via the system command line
   *                         shell, there is no access via a separate client program, when the
   *                         user input is terminated via the 'quit' command, the program is
   *                         exited.
   *
   * The 3rd argument is the server mode, the two valid values are:
   *
   *   PSHELL_NON_BLOCKING - A separate thread will be created to process user input, when
   *                         this function is called as non-blocking, the function will return
   *                         control to the calling context.
   *   PSHELL_BLOCKING     - No thread is created, all processing of user input is done within
   *                         this function call, it will never return control to the calling context.
   * 
   * The 4th and 5th arguments must be provided for a UDP or TCP server, for a LOCAL or
   * UNIX server they can be omitted, and if provided they will be ignored.
   * 
   * For the 4th argument, a valid IP address or hostname can be used.  There are also 3 special
   * "hostname" type identifiers defined as follows:
   *
   *   localhost - the loopback address (i.e. 127.0.0.1)
   *   myhost    - the hostname assigned to this host, this is usually defined in the
   *               /etc/hosts file and is assigned to the default interface
   *   anyhost   - all interfaces on a multi-homed host, for a single interface host
   *               this will yield the same results as using "myhost".
   *
   * Finally, the 5th argument is the desired port number (for UDP and TCP servers only).
   *
   * All of these arguments (except the initial name) can be overridden via the 'pshell.conf'
   * file on a per-server basis.
   *
   */
   
  pshell_startServer("pshellServerDemo", serverType, PSHELL_BLOCKING, "localhost", PSHELL_DEMO_PORT);

  return (0);
  
}