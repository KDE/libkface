/*
 * @file LibFaceConfig.h
 *
 * @section LICENSE
 *
 * This file is part of libface.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  Created on: Dec 27, 2009
 *  @author Alex Jironkin
 *  @author Gilles Caulier
 */

#ifndef LIBFACE_CONFIG_H
#define LIBFACE_CONFIG_H

// the configured options and settings for Tutorial

#define LibFace_VERSION_MAJOR @libface_MAJOR_VERSION@
#define LibFace_VERSION_MINOR @libface_MINOR_VERSION@
#define DEBUG @DEBUG_MODE@
#define LOG_LEVEL @LOG_LEVEL@
#define APPLE @IS_APPLE@
#define OPENCVDIR "@OpenCV_DIR@"

/* Shared library support, see http://gcc.gnu.org/wiki/Visibility */
#ifdef WIN32
# define FACE_IMPORT __declspec(dllimport)
# define FACE_EXPORT __declspec(dllexport)
# define FACE_DLLLOCAL
# define FACE_DLLPUBLIC
#else
# ifdef FACE_HAVE_GXXCLASSVISIBILITY
#  define FACE_IMPORT __attribute__ ((visibility("default")))
#  define FACE_EXPORT __attribute__ ((visibility("default")))
#  define FACE_DLLLOCAL __attribute__ ((visibility("hidden")))
#  define FACE_DLLPUBLIC __attribute__ ((visibility("default")))
# else
#  define FACE_IMPORT
#  define FACE_EXPORT
#  define FACE_DLLLOCAL
#  define FACE_DLLPUBLIC
# endif /* ! FACE_HAVE_GXXCLASSVISIBILITY */
#endif /* ! WIN32 */

/* Define EXIV2API for DLL builds */
#ifdef FACE_HAVE_DLL
# ifdef FACE_BUILDING_LIB
#  define FACEAPI FACE_EXPORT
# else
#  define FACEAPI FACE_IMPORT
# endif /* ! FACE_BUILDING_LIB */
#else
# define FACEAPI
#endif /* ! FACE_HAVE_DLL */

#endif /* LIBFACE_CONFIG_H */
