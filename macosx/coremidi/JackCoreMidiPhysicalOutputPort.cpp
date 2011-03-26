/*
Copyright (C) 2011 Devin Anderson

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <sstream>
#include <stdexcept>

#include "JackCoreMidiPhysicalOutputPort.h"
#include "JackCoreMidiUtil.h"

using Jack::JackCoreMidiPhysicalOutputPort;

JackCoreMidiPhysicalOutputPort::
JackCoreMidiPhysicalOutputPort(const char *alias_name, const char *client_name,
                               const char *driver_name, int index,
                               MIDIClientRef client,
                               MIDIPortRef internal_output, double time_ratio,
                               int realtime_priority, size_t max_bytes,
                               size_t max_messages):
    JackCoreMidiOutputPort(time_ratio, realtime_priority, max_bytes,
                           max_messages)
{
    MIDIEndpointRef destination = MIDIGetDestination(index);
    if (! destination) {
        // X: Can we get a better error message?
        std::stringstream stream;
        stream << "The destination at index '" << index
               << "' is not available";
        throw std::runtime_error(stream.str().c_str());
    }
    Initialize(alias_name, client_name, driver_name, index, destination);
    this->internal_output = internal_output;
}

JackCoreMidiPhysicalOutputPort::~JackCoreMidiPhysicalOutputPort()
{
    // Empty
}

bool
JackCoreMidiPhysicalOutputPort::SendPacketList(MIDIPacketList *packet_list)
{
    OSStatus status = MIDISend(internal_output, endpoint, packet_list);
    bool result = status == noErr;
    if (! result) {
        WriteMacOSError("JackCoreMidiPhysicalOutputPort::SendPacketList",
                        "MIDISend", status);
    }
    return result;
}
