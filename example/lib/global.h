/**
@file global.h
@brief Definitions and data structures for general use in all files
@author Joe Brown
*/
#ifndef GLOBAL_H
#define GLOBAL_H

#define MAGSWITCH_HW_R1
#define VERSION_STRING "0.0.1"
//#define DEBUG_ENABLED
//#define EXPERIMENT_BOARD
// for ascii art: http://patorjk.com/software/taag/

#include "msp430.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <assert.h>

enum {FALSE = 0, TRUE = 1};
enum {SUCCESS = 0, FAILURE = -1};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                __  ___
//               /  |/  /____ _ _____ _____ ____   _____
//              / /|_/ // __ `// ___// ___// __ \ / ___/
//             / /  / // /_/ // /__ / /   / /_/ /(__  )
//            /_/  /_/ \__,_/ \___//_/    \____//____/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#define _BV(bit)            (1<<(bit))
#define WD_STOP()           (WDTCTL = WDTPW + WDTHOLD)

// low power modes
#define SLEEP() LPM3
#define WAKE()  LPM3_EXIT

#ifdef DEBUG_ENABLED
    #define debug printf
#else
    #define debug(...)
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                  ______
//                 /_  __/__  __ ____   ___   _____
//                  / /  / / / // __ \ / _ \ / ___/
//                 / /  / /_/ // /_/ //  __/(__  )
//                /_/   \__, // .___/ \___//____/
//                     /____//_/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short int  uint16_t;
typedef signed short int    int16_t;
typedef unsigned long int   uint32_t;
typedef signed long int     int32_t;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                   ______ __        __            __
//                  / ____// /____   / /_   ____ _ / /_____
//                 / / __ / // __ \ / __ \ / __ `// // ___/
//                / /_/ // // /_/ // /_/ // /_/ // /(__  )
//                \____//_/ \____//_.___/ \__,_//_//____/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
extern volatile uint32_t g_clock_speed;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//            ____ ____     ______               ____ _
//           /  _// __ \   / ____/____   ____   / __/(_)____ _
//           / / / / / /  / /    / __ \ / __ \ / /_ / // __ `/
//         _/ / / /_/ /  / /___ / /_/ // / / // __// // /_/ /
//        /___/ \____/   \____/ \____//_/ /_//_/  /_/ \__, /
//                                                   /____/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//macros for IO config (with preprocessor abuse)
#define st(x) do{x} while(__LINE__ == -1)

enum IoDirection {INPUT = 0, OUTPUT = 1};
/**@brief Set the direction of an IO pin as INPUT or OUTPUT */
#define IO_DIRECTION(name,dir)          _IO_DIRECTION(name##_PORT,name##_PIN,dir)
/** @cond IGNORE_DOC */
#define _IO_DIRECTION(port,pin,dir)     st(__IO_DIRECTION(port,pin,dir);)
#define __IO_DIRECTION(port,pin,dir)    if (dir)\
                                        {\
                                            st((P##port##DIR |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##DIR &= ~_BV(pin)););\
                                        }

//st(P##port##DIR ^= (-dir ^ P##port##DIR) & _BV(pin);)
/** @endcond */

enum IoFunction {IO = 0, SPECIAL = 1};
/**@brief Set function of IO pin */
#define IO_FUNCTION(name,fun)           _IO_FUNCTION(name##_PORT,name##_PIN,fun)
/** @cond IGNORE_DOC */
#define _IO_FUNCTION(port, pin,fun)     st(__IO_FUNCTION(port,pin,fun);)
#define __IO_FUNCTION(port, pin,fun)    if (fun)\
                                        {\
                                            st((P##port##SEL0 |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##SEL0 &= ~_BV(pin)););\
                                        }
/** @endcond */


/**@brief Set auxilary function of IO pin */
#define IO_AUX_FUNCTION(name,fun)        _IO_AUX_FUNCTION(name##_PORT,name##_PIN,fun)
/** @cond IGNORE_DOC */
#define _IO_AUX_FUNCTION(port, pin,fun)  st(__IO_AUX_FUNCTION(port,pin,fun);)
#define __IO_AUX_FUNCTION(port, pin,fun) if (fun)\
                                         {\
                                             st((P##port##SEL1 |= _BV(pin)););\
                                         }\
                                         else\
                                         {\
                                             st((P##port##SEL1 &= ~_BV(pin)););\
                                         }
/** @endcond */

enum IoState {LOW = 0, HIGH = 1, TOGGLE = 2};
/**@brief Set state of IO pin */
#define IO_SET(name,val)                _IO_SET(name##_PORT,name##_PIN,val)
/** @cond IGNORE_DOC */
#define _IO_SET(port,pin,val)           st(__IO_SET(port,pin,val);)
#define __IO_SET(port,pin,val)          if (val == TOGGLE)\
                                        {\
                                            st((P##port##OUT ^= _BV(pin)););\
                                        }\
                                        else if (val == HIGH)\
                                        {\
                                            st((P##port##OUT |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##OUT &= ~_BV(pin)););\
                                        }
/** @endcond */

enum IoMode {DISABLED = 0, ENABLED = 1};
/**@brief Set resistor pull up or down for IO pin */
#define IO_RESISTOR(name,ren)           _IO_RESISTOR(name##_PORT,name##_PIN,ren)
/** @cond IGNORE_DOC */
#define _IO_RESISTOR(port,pin,ren)      st(__IO_RESISTOR(port,pin,ren);)
#define __IO_RESISTOR(port,pin,ren)     if (ren)\
                                        {\
                                            st((P##port##REN |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##REN &= ~_BV(pin)););\
                                        }
/** @endcond */

enum IoResistorState {PULL_DOWN = 0, PULL_UP = 1};
// Set pin as input before calling this macro
/**@brief Set resistor polarity (up or down) for IO pin */
#define IO_PULL(name,up)                IO_RESISTOR(name,ENABLED);\
                                        if (up)\
                                        {\
                                            IO_SET(name,HIGH);\
                                        }\
                                        else\
                                        {\
                                            IO_SET(name,LOW);\
                                        }

/**@brief Read in state of IO pin */
#define IO_IN(name)                     _IO_IN(name##_PORT,name##_PIN)
/** @cond IGNORE_DOC */
#define _IO_IN(port,pin)                __IO_IN(port,pin)
#define __IO_IN(port,pin)               !(P##port##IN & _BV(pin))
/** @endcond */

/**@brief Sent interrupt enable for IO pin */
#define IO_INT_EN(name,en)              _IO_INT_EN(name##_PORT,name##_PIN,en)
/** @cond IGNORE_DOC */
#define _IO_INT_EN(port,pin,en)         st(__IO_INT_EN(port,pin,en);)
#define __IO_INT_EN(port,pin,en)        if (en)\
                                        {\
                                            st((P##port##IE |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##IE &= ~_BV(pin)););\
                                        }
/** @endcond */

enum IoEdge {RISING_EDGE = 0, FALLING_EDGE = 1};
/**@brief Set rising or falling edge interrupt for IO pin */
#define IO_INT_EDGE(name,ed)            _IO_INT_EDGE(name##_PORT,name##_PIN,ed)
/** @cond IGNORE_DOC */
#define _IO_INT_EDGE(port,pin,ed)       st(__IO_INT_EDGE(port,pin,ed);)
#define __IO_INT_EDGE(port,pin,ed)      if (ed)\
                                        {\
                                            st((P##port##IES |= _BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##IES &= ~_BV(pin)););\
                                        }
/** @endcond */

#define CLEARED 1
/**@brief Set IO pin interrupt status */
#define IO_INT_FLAG(name,fl)            _IO_INT_FLAG(name##_PORT,name##_PIN,fl)
/** @cond IGNORE_DOC */
#define _IO_INT_FLAG(port,pin,fl)       st(__IO_INT_FLAG(port,pin,fl);)
#define __IO_INT_FLAG(port,pin,fl)      if (fl)\
                                        {\
                                            st((P##port##IFG &= ~_BV(pin)););\
                                        }\
                                        else\
                                        {\
                                            st((P##port##IFG |= _BV(pin)););\
                                        }
/** @endcond */

/**@brief Read IO pin interrupt status */
#define IO_INT_STAT(name)               _IO_INT_STAT(name##_PORT,name##_PIN)
/** @cond IGNORE_DOC */
#define _IO_INT_STAT(port,pin)          __IO_INT_STAT(port,pin)
#define __IO_INT_STAT(port,pin)         (P##port##IFG & _BV(pin))
/** @endcond */
#endif
