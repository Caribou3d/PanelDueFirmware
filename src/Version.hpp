/*
 * Version.hpp
 *
 *  Created on: 15 Mar 2018
 *      Author: David
 */

#ifndef SRC_VERSION_HPP_
#define SRC_VERSION_HPP_

#define VERSION_MAIN		"3.5.2"

#ifdef SUPPORT_ENCODER
# define VERSION_ENCODER	"+enc"
#else
# define VERSION_ENCODER
#endif

#ifdef DEVICE
# define STRINGIFY(x)	#x
# define TOSTRING(x)	STRINGIFY(x)
/* #define VERSION_DEVICE "-" TOSTRING(DEVICE) */
/* we only care about 5.0i and 7.0i and they get the same formware*/
#define VERSION_DEVICE
#else
# define VERSION_DEVICE
#endif

#define VERSION_TEXT VERSION_MAIN VERSION_ENCODER VERSION_DEVICE

#endif /* SRC_VERSION_HPP_ */
