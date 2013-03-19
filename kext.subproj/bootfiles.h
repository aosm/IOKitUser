/*
 * Copyright (c) 2008 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * FILE: bootfiles.h
 * AUTH: Soren Spies (sspies)
 * DATE: 22 March 2006 (Copyright Apple Computer, Inc)
 * DESC: constants for boot caches
 */

#ifndef __BOOTFILES_H__
#define __BOOTFILES_H__

#include <paths.h>

/* Boot != Root directories in Apple_Boot */
#define kBootDirR "com.apple.boot.R"
#define kBootDirP "com.apple.boot.P"
#define kBootDirS "com.apple.boot.S"

/* Boot != Root key for firmware's /chosen  */
#define kBootRootActiveKey      "bootroot-active"

/* Recovery OS directory in Apple_Boot */
#define kRecoveryBootDir        "com.apple.recovery.boot"

/* Recovery OS NVRAM variables & values */
// No recovery-boot-mode or Installer automation vars -> generic recovery
// with all functions available.  boot.efi boots the Recovery OS
// regardless of the value assigned to recovery-boot-mode.
#define kRecoveryBootVar        "recovery-boot-mode"
#define kRecoveryBootModeGuest      "guest"         // guest boot (usu. once)
#define kRecoveryBootModeLocked     "locked"        // system is FMM-locked
#define kRcevoeryBootModeRecovery   "recovery"      // OS->booter: generic pls

// Variable indicating that the user had trouble at EFI Login.
// Indicates to the OS that it should allow Bluetooth pairing, etc
// For now, this variable is set to "true" by the booter.
// The OS should unset this variable.
#define kRecoveryBootEFILoginHelpVar "recovery-boot-efilogin-help"

/* The kernel */
#define kDefaultKernel        "/mach_kernel"
#define kKernelSymfile        (_PATH_VARRUN "mach.sym")
// kKernelSymfile obsolete, remove when load.c deleted

/* The system extensions folder */
#define kSystemExtensionsDir  "/System/Library/Extensions"


/* The booter configuration file */
#define kBootConfig           "/Library/Preferences/SystemConfiguration/com.apple.Boot.plist"
#define kKernelFlagsKey       "Kernel Flags"
#define kMKextCacheKey        "MKext Cache"
#define kKernelNameKey        "Kernel"
#define kKernelCacheKey       "Kernel Cache"
#define kRootUUIDKey          "Root UUID"
#define kRootMatchKey         "Root Match"

#endif __BOOTFILES_H__
