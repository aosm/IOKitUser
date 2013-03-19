/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
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
/*
 * Copyright (c) 1998 Apple Computer, Inc.  All rights reserved. 
 *
 * HISTORY
 *
 */

#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/iokitmig.h>
#include <sys/types.h>
#include <unistd.h>

kern_return_t
IOFramebufferServerStart( void );

kern_return_t
IOHIDCreateSharedMemory( mach_port_t connect,
	unsigned int version )
{
    kern_return_t	err;
    unsigned int	len;

    IOFramebufferServerStart();

    len = 0;
    err = io_connect_method_scalarI_scalarO( connect, 0, /*index*/
                    &version, 1, NULL, &len);

    return( err);
}

kern_return_t
IOHIDSetEventsEnable( mach_port_t connect,
	boolean_t enable )
{
    kern_return_t	err;
    unsigned int	len;

    len = 0;
    err = io_connect_method_scalarI_scalarO( connect, 1, /*index*/
                    &enable, 1, NULL, &len);

    return( err);
}

kern_return_t
IOHIDSetCursorEnable( mach_port_t connect,
	boolean_t enable )
{
    kern_return_t	err;
    unsigned int	len;

    len = 0;
    err = io_connect_method_scalarI_scalarO( connect, 2, /*index*/
                    &enable, 1, NULL, &len);

    return( err);
}

/* DEPRECATED form of IOHIDPostEvent().
kern_return_t
IOHIDPostEvent( mach_port_t connect,
                int type, IOGPoint location, NXEventData *data,
                boolean_t setCursor, int flags, boolean_t setFlags)
*/

kern_return_t
IOHIDPostEvent( io_connect_t        connect,
                UInt32              eventType,
                IOGPoint            location,
                const NXEventData * eventData,
                UInt32              eventDataVersion,
                IOOptionBits        eventFlags,
                IOOptionBits        options )
{
    kern_return_t       err;
    unsigned int        len;
    int *               eventPid = 0;
    int                 dataSize = sizeof(struct evioLLEvent) + sizeof(int);
    char                data[dataSize];
    struct evioLLEvent* event;
    UInt32              eventDataSize = sizeof(NXEventData);

    bzero(data, dataSize);
    
    event = (struct evioLLEvent*)data;
    
    event->type      = eventType;
    event->location  = location;
    event->flags     = eventFlags;
    event->setFlags  = options;
    event->setCursor = options & (kIOHIDSetCursorPosition | kIOHIDSetRelativeCursorPosition);
    
    eventPid = (int *)(event + 1);
    *eventPid = getpid();

    if ( eventDataVersion < 2 )
    {
        // Support calls from legacy IOHIDPostEvent clients.
        // 1. NXEventData was 32 bytes long.
        // 2. eventDataVersion was (boolean_t) setCursor
        eventDataSize   = 32;
        event->setCursor = eventDataVersion; // 0 or 1
    }

    if ( eventDataSize < sizeof(event->data) )
    {
        bcopy( eventData, &(event->data), eventDataSize );
        bzero( ((UInt8 *)(&(event->data))) + eventDataSize,
               sizeof(event->data) - eventDataSize );
    }
    else
        bcopy( eventData, &event->data, sizeof(event->data) );

    len = 0;
    err = io_connect_method_structureI_structureO(
             connect,
             3,                /* index       */
             data,             /* input       */
             dataSize,         /* inputCount  */
             NULL,             /* output      */
             &len);            /* outputCount */
             
    return (err);
}

extern kern_return_t
IOHIDSetCursorBounds( io_connect_t connect, const IOGBounds * bounds )
{
    IOByteCount	len = 0;

    return( IOConnectMethodStructureIStructureO( connect, 6, /*index*/
                    sizeof( *bounds), &len,
                    bounds, NULL ));
}

kern_return_t
IOHIDSetMouseLocation( mach_port_t connect,
	int x, int y)
{
    kern_return_t	err;
    unsigned int	len;
    IOGPoint *		loc;
    int             dataSize = sizeof(IOGPoint) + sizeof(int);
    char            data[dataSize];
    int *           eventPid = 0;
        
    bzero(data, dataSize);
    
    loc = (IOGPoint *)data;
    
    loc->x = x;
    loc->y = y;

    eventPid = (int *)(loc + 1);
    *eventPid = getpid();

    len = 0;
    err = io_connect_method_structureI_structureO( connect, 4, /*index*/
                    data, dataSize, NULL, &len);

    return( err);
}

kern_return_t
IOHIDGetButtonEventNum( mach_port_t connect,
	NXMouseButton button, int * eventNum )
{
    kern_return_t	err;
    unsigned int	len;

    len = 1;
    err = io_connect_method_scalarI_scalarO( connect, 5, /*index*/
                    (int *)&button, 1, (void *)eventNum, &len);

    return( err);
}
