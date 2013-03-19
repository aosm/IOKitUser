/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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

#include <CoreFoundation/CoreFoundation.h>
#include "IOSystemConfiguration.h"
#include <IOKit/pwr_mgt/IOPMLibPrivate.h>
#include "IOPowerSources.h"
#include "IOPowerSourcesPrivate.h"
#include "IOPSKeys.h"

/* IOPSCopyInternalBatteriesArray
 *
 * Argument: 
 *	CFTypeRef power_sources: The return value from IOPSCoyPowerSourcesInfo()
 * Return value:
 * 	CFArrayRef: all the batteries we found
 *			NULL if none are found
 */
extern CFArrayRef
IOPSCopyInternalBatteriesArray(CFTypeRef power_sources)
{
    CFArrayRef			array = isA_CFArray(IOPSCopyPowerSourcesList(power_sources));
    CFMutableArrayRef   ret_arr;
    CFTypeRef			name = NULL;
    CFDictionaryRef		ps;
    CFStringRef			transport_type;
    int				    i, count;

    if(!array) return NULL;
    count = CFArrayGetCount(array);
    name = NULL;

    ret_arr = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if(!ret_arr) goto exit;

    // Iterate through power_sources
    for(i=0; i<count; i++) {
        name = CFArrayGetValueAtIndex(array, i);
        ps = isA_CFDictionary(IOPSGetPowerSourceDescription(power_sources, name));
        if(ps) {
            // Return the first power source that is an internal battery
            transport_type = isA_CFString(CFDictionaryGetValue(ps, CFSTR(kIOPSTransportTypeKey)));
            if(transport_type && CFEqual(transport_type, CFSTR(kIOPSInternalType)))
            {
                CFArrayAppendValue(ret_arr, name);
            }
        }
    }

    if(0 == CFArrayGetCount(ret_arr)) {
        CFRelease(ret_arr);
        ret_arr = NULL;
    }

exit:
    CFRelease(array);
    return ret_arr;
}


/* IOPSCopyUPSArray
 *
 * Argument: 
 *	CFTypeRef power_sources: The return value from IOPSCoyPowerSourcesInfo()
 * Return value:
 * 	CFArrayRef: all the UPS's we found
 *			NULL if none are found
 */
 extern CFArrayRef
IOPSCopyUPSArray(CFTypeRef power_sources)
{
    CFArrayRef			array = isA_CFArray(IOPSCopyPowerSourcesList(power_sources));
    CFMutableArrayRef   ret_arr;
    CFTypeRef			name = NULL;
    CFDictionaryRef		ps;
    CFStringRef			transport_type;
    int				    i, count;

    if(!array) return NULL;
    count = CFArrayGetCount(array);
    name = NULL;

    ret_arr = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if(!ret_arr) goto exit;

    // Iterate through power_sources
    for(i=0; i<count; i++) {
        name = CFArrayGetValueAtIndex(array, i);
        ps = isA_CFDictionary(IOPSGetPowerSourceDescription(power_sources, name));
        if(ps) {
            // Return the first power source that is an internal battery
            transport_type = isA_CFString(CFDictionaryGetValue(ps, CFSTR(kIOPSTransportTypeKey)));
            if(transport_type && ( CFEqual(transport_type, CFSTR(kIOPSSerialTransportType)) ||
                CFEqual(transport_type, CFSTR(kIOPSUSBTransportType)) ||
                CFEqual(transport_type, CFSTR(kIOPSNetworkTransportType)) ) )
            {
                CFArrayAppendValue(ret_arr, name);
            }
        }
    }

    if(0 == CFArrayGetCount(ret_arr)) {
        CFRelease(ret_arr);
        ret_arr = NULL;
    }

exit:
    CFRelease(array);
    return ret_arr;
}

/* IOPSGetActiveUPS
 *
 * Argument: 
 *	CFTypeRef power_sources: The return value from IOPSCoyPowerSourcesInfo()
 * Return value:
 * 	CFTypeRef (actually a CFStringRef): the UPS we're paying attention to, since we only
 *              support one UPS at a time.
 *			NULL if none are found
 */
extern CFTypeRef  
IOPSGetActiveUPS(CFTypeRef ps_blob)
{
    CFTypeRef       ret_ups;
    CFArrayRef      ups_arr;
    
    ups_arr = IOPSCopyUPSArray(ps_blob);
    if(!ups_arr)
    {
        return NULL;    
    }

    ret_ups = CFArrayGetValueAtIndex(ups_arr, 0);
    
    CFRelease(ups_arr);

    return ret_ups;
}

/* IOPSGetActiveBattery
 *
 * Argument: 
 *	CFTypeRef power_sources: The return value from IOPSCoyPowerSourcesInfo()
 * Return value:
 * 	CFTypeRef (actually a CFStringRef): the UPS we're paying attention to, since we only
 *              support one UPS at a time.
 *			NULL if none are found
 */
extern CFTypeRef  
IOPSGetActiveBattery(CFTypeRef ps_blob)
{
    CFTypeRef       ret_ups;
    CFArrayRef      ups_arr;
    
    ups_arr = IOPSCopyInternalBatteriesArray(ps_blob);
    if(!ups_arr)
    {
        return NULL;    
    }

    ret_ups = CFArrayGetValueAtIndex(ups_arr, 0);
    
    CFRelease(ups_arr);

    return ret_ups;
}

static CFStringRef getPowerSourceState(CFTypeRef blob, CFTypeRef id)
{
    CFDictionaryRef the_dict = IOPSGetPowerSourceDescription(blob, id);
    return CFDictionaryGetValue(the_dict, CFSTR(kIOPSPowerSourceStateKey));
}

/* _getProvidingPowerSourceType
 * Argument: 
 *  ps_blob: as returned from IOPSCopyPowerSourcesInfo()
 * Returns: 
 *  The current system power source.
 *  CFSTR("AC Power"), CFSTR("Battery Power"), CFSTR("UPS Power")
 */
extern CFStringRef IOPSGetProvidingPowerSourceType(CFTypeRef ps_blob)
{
    // CFArrayRef      batt_arr = NULL;
    // CFArrayRef      ups_arr = NULL;
    CFTypeRef       the_ups = NULL;
    CFTypeRef       the_batt = NULL;
    CFStringRef     ps_state = NULL;


    if(kCFBooleanFalse == IOPSPowerSourceSupported(ps_blob, CFSTR(kIOPMBatteryPowerKey)))
    {
        if(kCFBooleanFalse == IOPSPowerSourceSupported(ps_blob, CFSTR(kIOPMUPSPowerKey))) {
            // no batteries, no UPS -> AC Power
            return CFSTR(kIOPMACPowerKey);
        } else {
            // optimization opportunity: needless loops inside IOPSGetActiveUPS
            the_ups = IOPSGetActiveUPS(ps_blob);
            if(!the_ups) return CFSTR(kIOPMACPowerKey);
            ps_state = getPowerSourceState(ps_blob, the_ups);
            if(ps_state && CFEqual(ps_state, CFSTR(kIOPSACPowerValue)))
            {
                // no batteries, yes UPS, UPS is running off of AC power -> AC Power
                return CFSTR(kIOPMACPowerKey);
            } else if(ps_state && CFEqual(ps_state, CFSTR(kIOPSBatteryPowerValue)))
            {
                // no batteries, yes UPS, UPS is running off of Battery power -> UPS Power
                return CFSTR(kIOPMUPSPowerKey);
            }
            
        }
        // Error in the data we were passed
        return CFSTR(kIOPMACPowerKey);
    } else {
        // Optimization opportunity: needless loops inside IOPSGetActiveBattery
        the_batt = IOPSGetActiveBattery(ps_blob);
        if(!the_batt) return CFSTR(kIOPMACPowerKey);
        ps_state = getPowerSourceState(ps_blob, the_batt);
        if(ps_state && CFEqual(ps_state, CFSTR(kIOPSBatteryPowerValue)))
        {
            // Yes batteries, yes running on battery power -> Battery power
            return CFSTR(kIOPMBatteryPowerKey);
        } else {
            // batteries are on AC power. let's check UPS.
            // optimize.
            if(kCFBooleanFalse == IOPSPowerSourceSupported(ps_blob, CFSTR(kIOPMUPSPowerKey)))
            {
                // yes batteries on AC power, no UPS -> AC Power
                return CFSTR(kIOPMACPowerKey);
            } else {
                the_ups = IOPSGetActiveUPS(ps_blob);
                if(!the_ups) return CFSTR(kIOPMACPowerKey);
                ps_state = getPowerSourceState(ps_blob, the_ups);
                if(ps_state && CFEqual(ps_state, CFSTR(kIOPSBatteryPowerValue)))                
                {
                    // yes batteries on AC power, UPS is on battery power -> UPS Power
                    return CFSTR(kIOPMUPSPowerKey);
                } else if(ps_state && CFEqual(ps_state, CFSTR(kIOPSACPowerValue)))
                {
                    // yes batteries on AC Power, UPS is on AC Power -> AC Power
                    return CFSTR(kIOPMACPowerKey);
                }
            }
        }
        // Error in the data we were passed.
//        syslog(LOG_INFO, "PowerManagement: unexpected point 3 reached in _getProvidingPowerSourceType\n");
    }

    // Should not reach this point. Return something safe.
    return CFSTR(kIOPMACPowerKey);
}

/***
 * int powerSourceSupported(CFStringRef)
 * takes: CFSTR of kIOPMACPowerKey, kIOPMBatteryPowerKey, kIOPMUPSPowerKey
 * returns true if this machine supports (has) that power type.
 */
extern CFBooleanRef IOPSPowerSourceSupported(CFTypeRef ps_blob, CFStringRef ps_type)
{
    CFBooleanRef       ret = kCFBooleanFalse;

    if(!ps_blob) return kCFBooleanFalse;

    if(!isA_CFString(ps_type)) 
    {
        ret = kCFBooleanFalse;
    } else if(CFEqual(ps_type, CFSTR(kIOPMACPowerKey))) 
    {
        ret = kCFBooleanTrue;
    } else if(CFEqual(ps_type, CFSTR(kIOPMBatteryPowerKey))) 
    {
        if(IOPSGetActiveBattery(ps_blob))
        {
            ret = kCFBooleanTrue;
        } else {
            ret = kCFBooleanFalse;
        }
    } else if(CFEqual(ps_type, CFSTR(kIOPMUPSPowerKey))) 
    {
        if(IOPSGetActiveUPS(ps_blob))
        {
            ret = kCFBooleanTrue;
        } else {
            ret = kCFBooleanFalse;
        }
    }
    
    return ret;
}


