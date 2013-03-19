/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _IOPMLibPrivate_h_
#define _IOPMLibPrivate_h_

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CFArray.h>
#include <IOKit/pwr_mgt/IOPMLibDefs.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define kIOPMServerBootstrapName    "com.apple.PowerManagement.control"

// AutoWake API
// For internal use communicating between IOKitUser and PM configd
// The preferences file
#define kIOPMAutoWakePrefsPath      "com.apple.AutoWake.xml"

// Type arguments to IOPMSchedulePowerEvent for internal use
// These types are not cancellable, nor will they appear in any lists of
// scheduled arguments from IOPMCopyScheduledEvents. These are not acceptable
// types to pass as Repeating events.

// The 'date' argument to IOPMSchedulePowerEvent is an absolute one for 
// 'AutoWakeScheduleImmediate' and 'AutoPowerScheduleImmediate'. The effect
// of these WakeImmediate and PowerOnImmediate types is to schedule the
// wake/power event directly with the wake/power controller, ignoring all OS
// queueing and management. This will override a previously scheduled wake event
// by another application, should one exist. Recommended for testing only. 

#define kIOPMAutoWakeScheduleImmediate      "WakeImmediate"
#define kIOPMAutoPowerScheduleImmediate     "PowerOnImmediate"

// The 'date' argument to IOPMSchedulePowerEvent is an relative one to 
// "right now," for 'AutoWakeScheduleImmediate' and 'AutoPowerScheduleImmediate'
//
// e.g. In this case, we're setting the system to wake from sleep exactly 10
// seconds after the system completes going to sleep. We're passing in a date
// 10 seconds past "right now", but the wakeup controller interprets this as 
// relative to sleep time.
//
//  d = CFDateCreate(0, CFAbsoluteGetCurrent() + 10.0);
//  IOPMSchedulePowerEvent(d, CFSTR("SleepCycler"), 
//                            CFSTR(kIOPMAutoWakeRelativeSeconds) );

#define kIOPMAutoWakeRelativeSeconds        kIOPMSettingDebugWakeRelativeKey
#define kIOPMAutoPowerRelativeSeconds       kIOPMSettingDebugPowerRelativeKey


/**************************************************
*
* Energy Saver Preferences
*
**************************************************/
#define kIOPMDynamicStoreSettingsKey    "State:/IOKit/PowerManagement/CurrentSettings"
#define kIOPMDefaultPreferencesKey      "Defaults"

#define kIOPMUPSPowerKey                                "UPS Power"
#define kIOPMBatteryPowerKey                            "Battery Power"
#define kIOPMACPowerKey                                 "AC Power"

// units - CFNumber in minutes
#define kIOPMDisplaySleepKey                            "Display Sleep Timer"
// units - CFNumber in minutes
#define kIOPMDiskSleepKey                               "Disk Sleep Timer"
// units - CFNumber in minutes
#define kIOPMSystemSleepKey                             "System Sleep Timer"	

// units - CFNumber 0/1
#define kIOPMReduceSpeedKey                             "Reduce Processor Speed"
// units - CFNumber 0/1
#define kIOPMDynamicPowerStepKey                        "Dynamic Power Step"
// units - CFNumber 0/1
#define kIOPMWakeOnLANKey                               "Wake On LAN"
// units - CFNumber 0/1
#define kIOPMWakeOnRingKey                              "Wake On Modem Ring"
// units - CFNumber 0/1
#define kIOPMRestartOnPowerLossKey                      "Automatic Restart On Power Loss"
// units - CFNumber 0/1
#define kIOPMWakeOnACChangeKey                          "Wake On AC Change"
// units - CFNumber 0/1
#define kIOPMSleepOnPowerButtonKey                      "Sleep On Power Button"
// units - CFNumber 0/1
#define kIOPMWakeOnClamshellKey                         "Wake On Clamshell Open"
// units - CFNumber 0/1
#define kIOPMReduceBrightnessKey                        "ReduceBrightness"
// units - CFNumber 0/1
#define kIOPMDisplaySleepUsesDimKey                     "Display Sleep Uses Dim"
// units - CFNumber 0/1
#define kIOPMMobileMotionModuleKey                      "Mobile Motion Module"
// units - CFNumber 0/1
#define kIOPMTTYSPreventSleepKey                        "TTYSPreventSleep"

typedef void (*IOPMPrefsCallbackType)(void *context);

    /*!
@function IOPMPrefsNotificationCreateRunLoopSource
@abstract  Returns a CFRunLoopSourceRef that notifies the caller when any PM preferences
    file changes - including Energy Saver or UPS changes.
@param callback A function to be called whenever a prefs file changes.
@param context Any user-defined pointer, passed to the IOPowerSource callback.
@result Returns NULL if an error was encountered, otherwise a CFRunLoopSource. Caller must
    release the CFRunLoopSource.
    */
CFRunLoopSourceRef IOPMPrefsNotificationCreateRunLoopSource(IOPMPrefsCallbackType, void *);

/* Structure of a "pm preferences" dictionary, as referenced below.
 *
 * PMPreferencesDictionary = 
 * {
 *      "AC Power" = PowerSourcePreferencesDictionary
 *      "Battery Power" = PowerSourcePreferencesDictionary
 *      "UPS Power" = PowerSourcePreferencesDictionary
 *      kIOPMDefaultPreferencesKey = true/false
 * }
 *
 * PowerSourcePreferencesDictionary = 
 * {
 *      kIOPMDiskSleepKey = 0 and up
 *      kIOPMSystemSleepKey = 0 and up
 *      kIOPMDisplaySleepKey = 0 and up
 *
 *      kIOPMWakeOnLANKey = 0/1
 *      kIOPMWakeOnRingKey = 0/1
 *      kIOPMWakeOnACChangeKey = 0/1
 *      ... etc
 * }
 *
 *
 * Notes: 
 * * If battery power or UPS power are not present on a system, they will
 *   be absent from the PM Preferences dictionary.
 * * If a given setting, like ReduceBrightness is unsupported on a given
 *   system, it will not appear in the PowerSourcePreferencesDictionary.
 * * If the property kIOPMDefaultPreferencesKey is present and set to true,
 *   then the user has not specified any custom PM preferences. These are all
 *   "hardcoded" default properties. As such, the settings in a default custom
 *   dict do NOT factor in power profile settings.
 *
 * * See IOPMCopyActivePMPreferences() for settings that account for PM profile
 *   selections.
 */

    /*!
@function IOPMCopyCustomPMPreferences.
@abstract Returns a dictionary of user-selected custom PM preferences. 
    The CFString key kIOPMDefaultPreferencesKey will be present in the top-level 
    dictionary if the returned settings are defaults, as in the case of a clean 
    install
@result Returns a CFDictionaryRef or NULL. Caller must CFRelease the dictionary.
     */
CFDictionaryRef     IOPMCopyCustomPMPreferences(void);

/*!
@function IOPMCopyPMPreferences
@abstract Identical to IOPMCopyCustomPMPreferences; Renamed to 
    IOPMCopyCustomPMPreferences for clarity.
 */
CFMutableDictionaryRef IOPMCopyPMPreferences(void);


/*!
@function IOPMCopyActivePMPreferences
@abstract Returns the _actual_ PM settings _currently_ in use by the system.
@discussion Incorporates the active power profiles as well as custom settings.
        Does not return any settings unsupported on the running computer.
 */
CFDictionaryRef     IOPMCopyActivePMPreferences(void);

/*!
@function IOPMCopyUnabridgedActivePMPreferences
@abstract Returns the _actual_ PM settings _currently_ in use by the system.
@discussion Behaves identically to IOPMCopyActivePMPreferences, except 
        IOPMCopyUnabridgedActivePMPreferences returns all settings, including
        those settings unsupported on the running machine.
        i.e. the returned dictionary will include a setting for WakeOnRing
        on a computer without a modem.
 */
CFDictionaryRef     IOPMCopyUnabridgedActivePMPreferences(void);


    /*!
@function IOPMSetCustomPMPreferences
@abstract Writes a PM preferences dictionary to disk.
    Also activates these preferences and sends notifications to interested 
    applications via SystemConfiguration.
@param ESPrefs  Dictionary of Power Management preferences to write out to disk.
@result Returns kIOReturnSuccess or an error condition if request failed.
     */
IOReturn IOPMSetCustomPMPreferences(CFDictionaryRef ESPrefs);

/*!
@function IOPMSetPMPreferences
@abstract Identical to IOPMSetCustomPMPreferences. Renamed to 
    IOPMSetCustomPMPreferencs for clarity.
 */
IOReturn IOPMSetPMPreferences(CFDictionaryRef ESPrefs);

    /*!
@function IOPMActivatePMPreference.
@abstract Activates the set of preferences called "name." Sets idle spin down timers and other
    Energy Saver settings according to profile name.
@param SystemProfiles  The dictionary of preferences from IOPMCopyPMPreferences
@param profile The name of the set of preferences defined in ESPrefs to activate.
@param removeUnsupportedSettings Specifies whether to send settings to kernel for
settings that are currently unsupported (i.e. WakeOnRing with no modem present). True
means do not activate unsupported settings, false means DO activate unsupported settings.
@result Returns kIOReturnSuccess or an error condition if request failed.
     */
IOReturn IOPMActivatePMPreference(
    CFDictionaryRef SystemProfiles, 
    CFStringRef profile,
    bool removeUnsupportedSettings);

    /*!
@function IOPMFetaureIsAvailable
@abstract Identifies whether a PM feature is supported on this platform.
@param feature The CFSTR() feature to check availability of
@param power_source CFSTR(kIOPMACPowerKey) or CFSTR(kIOPMBatteryPowerKef). Doesn't 
    matter for most features, but a few are power source dependent.
@result Returns true if supported, false otherwise.
     */
bool IOPMFeatureIsAvailable(CFStringRef feature, CFStringRef power_source);

/**************************************************/

    /*!
    @function IOPMSleepSystemWithOptions
    @abstract Request that the system initiate sleep.
    @discussion For security purposes, caller must be root or the console user.
    @param userClient  Port used to communicate to the kernel,  from IOPMFindPowerManagement.
    @param sleepOptions a dictionary defining optional arguments to sleep.
    @result Returns kIOReturnSuccess or an error condition if request failed.
     */
IOReturn IOPMSleepSystemWithOptions ( io_connect_t userClient, CFDictionaryRef sleepOptions );

/**************************************************/

/* 
 * kIOPMSystemSleepAvailableAtAll
 *      System Power Setting (not power source specific)
 *      value = true/false
 */
#define	kIOPMSleepDisabledKey	CFSTR("SleepDisabled")

    /*!
@function IOPMCopySystemPowerSettings
@abstract Returns System power settings. 
          System-wide power settings are not power source dependent.
     */
CFDictionaryRef IOPMCopySystemPowerSettings( void );

    /*
@function IOPMSetSystemPowerSetting
@abstract Set a system-wide power management setting
@param key Setting name
@param value Setting value
@result kIOReturnSuccess; or IOReturn error otherwise
     */
IOReturn IOPMSetSystemPowerSetting( CFStringRef key, CFTypeRef value );

    /*!
@function IOPMActivateSystemPowerSettings
@abstract Re-send system power settings to kernel.
@result   kIOReturnSuccess on success; IOReturn error otherwise
     */
IOReturn IOPMActivateSystemPowerSettings( void );


/**************************************************
*
* Power Profiles (use in combination with Energy Saver Preferences above)
*
**************************************************/
#define kIOPMCustomPowerProfile         -1
#define kIOPMNumPowerProfiles           5

/*! @function IOPMCopyPowerProfilesInfo
    @abstract Returns all power profiles and their corresponding Energy Settings.
    @discussion The array return value contains 5 dictionaries.
        - Entry 0 represents the "highest power savings", and entry 4 represents "Highest performance"
        - Each entry in the array is a "Power Profile" dictionary, which in turn contains individual
        dictionaries for AC Power, Battery Power, and UPS Power. Each of these per-power source
        dictionaries contains a mapping of Energy Settings to their values.
        Intended clients: Energy Saver Prefs, Battery Monitor, and pmset.
        - Use IOPMCopyPMPreferences() to read the custom profile.
        - Unsupported features and unsupported power sources will not be present in the returned data.
    @result NULL on error, a CFArrayRef on success.
        Caller must CFRelease() the return value when done.
*/
CFArrayRef          IOPMCopyPowerProfiles(void);

/*! @function IOPMCopyActivePowerProfile
    @abstract Returns the index of the currently active profile, or -1 if Custom preferences are active.
    @result A CFDictionary containing 1 or more entries mapping Power Sources to the currently
            selected profiles for each power source. kIOPMUPSPowerKey, kIOPMACPowerKey, kIOPMBatteryPowerKey
            With corresponding CFNumber value specifying an index falling somewhere within the 
            IOPMCopyPowerProfiles() array
            OR integer value kIOPMCustomPowerProfile.
*/
CFDictionaryRef     IOPMCopyActivePowerProfiles(void);

/*! @function IOPMSetActivePowerProfile
    @abstract Activates the specified power profile, or the user's custom defined profile.
    @discussion Caller must have root, or admin privileges, or be the console user.
        Selects and activates a system power profile per power-source.
        - Use IOPMSetPMPreferences() to re-program the custom profile.
        - This call triggers notifications to all clients of IOPMPrefsNotificationCreateRunLoopSource()
    @param which_profile A CFDictionary specifying which profile to use for each power source.
        Keys should be CFSTR: kIOPMUPSPowerKey, kIOPMACPowerKey, kIOPMBatteryPowerKey
        Value should be a CFNumber (IntType) with value -1 to 4
    @result kIOReturnNotPrivileged if caller does not have permission.
        Caller must CFRelease() the return value when done accessing it.
*/
IOReturn            IOPMSetActivePowerProfiles(CFDictionaryRef which_profile);


/**************************************************
*
* Repeating Sleep/Wake/Shutdown/Restart API
*
**************************************************/

// Keys to index into CFDictionary returned by IOPSCopyRepeatingPowerEvents()
#define     kIOPMRepeatingPowerOnKey        "RepeatingPowerOn"
#define     kIOPMRepeatingPowerOffKey       "RepeatingPowerOff"

#define     kIOPMAutoSleep                  "sleep"
#define     kIOPMAutoShutdown               "shutdown"

// Keys to "days of week" bitfield for IOPMScheduleRepeatingPowerEvent()
enum {
    kIOPMMonday         = 1 << 0,
    kIOPMTuesday        = 1 << 1,
    kIOPMWednesday      = 1 << 2,
    kIOPMThursday       = 1 << 3,
    kIOPMFriday         = 1 << 4,
    kIOPMSaturday       = 1 << 5,
    kIOPMSunday         = 1 << 6
};

// Keys to index into sub-dictionaries of the dictionary returned by IOPSCopyRepeatingPowerEvents
// Absolute time to schedule power on (stored as a CFNumberRef, type = kCFNumberIntType)
//#define kIOPMPowerOnTimeKey                 "time"
// Bitmask of days to schedule a wakeup for (CFNumberRef, type = kCFNumberIntType)
#define kIOPMDaysOfWeekKey                  "weekdays"
// Type of power on event (CFStringRef)
//#define kIOPMTypeOfPowerOnKey               "typeofwake"

/*  @function IOPMScheduleRepeatingPowerEvent
 *  @abstract Schedules a repeating sleep, wake, shutdown, or restart
 *  @discussion Private API to only be used by Energy Saver preferences panel. Note that repeating sleep & wakeup events are valid together, 
 *              and shutdown & power on events are valid together, but you cannot mix sleep & power on, or shutdown & wakeup events.
 *              Every time you call IOPMSchedueRepeatingPowerEvent, we will cancel all previously scheduled repeating events of that type, and any
 *              scheduled repeating events of "incompatible" types, as I just described.
 *  @param  events A CFDictionary containing two CFDictionaries at keys "RepeatingPowerOn" and "RepeatingPowerOff".
                Each of those dictionaries contains keys for the type of sleep, the days_of_week, and the time_of_day. These arguments specify the
                time, days, and type of power events.
 *  @result kIOReturnSuccess on success, kIOReturnError or kIOReturnNotPrivileged otherwise.
 */
IOReturn IOPMScheduleRepeatingPowerEvent(CFDictionaryRef events);

/*  @function IOPMCopyRepeatingPowerEvents
 *  @abstract Gets the system
 *  @discussion Private API to only be used by Energy Saver preferences panel. Copies the system's current repeating power on
                and power off events.
                The returned CFDictionary contains two CFDictionaries at keys "RepeatingPowerOn" and "RepeatingPowerOff".
                Each of those dictionaries contains keys for the type of sleep, the days_of_week, and the time_of_day.
 *  @result NULL on failure, CFDictionary (possibly empty) otherwise.
 */
 CFDictionaryRef IOPMCopyRepeatingPowerEvents(void);

/*  @function IOPMScheduleRepeatingPowerEvent
 *  @abstract Cancels all repeating power events
 *  @result kIOReturnSuccess on success, kIOReturnError or kIOReturnNotPrivileged otherwise.
 */ 
IOReturn IOPMCancelAllRepeatingPowerEvents(void);

/**************************************************
*
* Assertions
* Private assertions; not published publicly
*
**************************************************/
// Keeps the CPU at its highest level
#define kIOPMAssertionTypeNeedsCPU              CFSTR("CPUBoundAssertion")
#define kIOPMCPUBoundAssertion                  kIOPMAssertionTypeNeedsCPU

// Disables AC Power Inflow (requires root to initiate)
#define kIOPMAssertionTypeDisableInflow         CFSTR("DisableInflow")
#define kIOPMInflowDisableAssertion             kIOPMAssertionTypeDisableInflow

// Disables battery charging (requires root to initiate)
#define kIOPMAssertionTypeInhibitCharging       CFSTR("ChargeInhibit")
#define kIOPMChargeInhibitAssertion             kIOPMAssertionTypeInhibitCharging

// Disables low power battery warnings
#define kIOPMAssertionTypeDisableLowBatteryWarnings     CFSTR("DisableLowPowerBatteryWarnings")
#define kIOPMDisableLowBatteryWarningsAssertion         kIOPMAssertionTypeDisableLowBatteryWarnings

// Once initially asserted, the machine may only idle sleep while this assertion
// is asserted. For embedded use only.
#define kIOPMAssertionTypeEnableIdleSleep           CFSTR("EnableIdleSleep")

__END_DECLS

#endif // _IOPMLibPrivate_h_

