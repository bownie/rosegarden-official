// -*- c-indentation-style:"stroustrup" c-basic-offset: 4 -*-
/*
    Rosegarden-4
    A sequencer and musical notation editor.

    This program is Copyright 2000-2004
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include <vector>
#include <string>

#include "XmlExportable.h"

// An Instrument on needs to implement these to render an instance
// of the plugin at the sequencer.
//

#ifndef _AUDIOPLUGININSTANCE_H_
#define _AUDIOPLUGININSTANCE_H_

namespace Rosegarden
{

typedef float PortData;

class PluginPort
{
public:
    typedef enum
    {
    // Currently things will go wrong if these values differ
    // from the same constants in LADSPA.  That's bad.
	//!!! no longer true
        Input    = 0x01,
        Output   = 0x02,
        Control  = 0x04,
        Audio    = 0x08
    } PortType;

    typedef enum
    {
    // These are our own, though.
        NoHint      = 0x00,
	Toggled     = 0x01,
	Integer     = 0x02,
	Logarithmic = 0x04
    } PortDisplayHint;

    PluginPort(int number,
               std::string m_name,
               PortType type,
               PortDisplayHint displayHint,
               PortData lowerBound,
               PortData upperBound,
	       PortData defaultValue);

    int getNumber() const { return m_number; }
    std::string getName() const { return m_name; }
    PortType getType() const { return m_type; }
    PortDisplayHint getDisplayHint() const { return m_displayHint; }
    PortData getLowerBound() const { return m_lowerBound; }
    PortData getUpperBound() const { return m_upperBound; }
    PortData getDefaultValue() const { return m_default; }

protected:

    int             m_number;
    std::string     m_name;
    PortType        m_type;
    PortDisplayHint m_displayHint;
    PortData        m_lowerBound;
    PortData        m_upperBound;
    PortData        m_default;
};

class PluginPortInstance
{
public:
    PluginPortInstance(unsigned int n,
                       float v):number(n), value(v) {;}

    int number;
    PortData value;
};

typedef std::vector<PluginPortInstance*>::iterator PortInstanceIterator;

class AudioPluginInstance : public XmlExportable
{
public:
    static const unsigned int SYNTH_POSITION;

    AudioPluginInstance(unsigned int position);

    AudioPluginInstance(std::string identifier,
			unsigned int position);

    void setIdentifier(std::string identifier) { m_identifier = identifier; }
    std::string getIdentifier() const { return m_identifier; }

    void setPosition(unsigned int position) { m_position = position; }
    unsigned int getPosition() const { return m_position; }

    PortInstanceIterator begin() { return m_ports.begin(); }
    PortInstanceIterator end() { return m_ports.end(); }

    // Port management
    //
    void addPort(int number, PortData value);
    bool removePort(int number);
    PluginPortInstance* getPort(int number);
    void clearPorts();

    unsigned int getPortCount() const { return m_ports.size(); }

    // export
    std::string toXmlString();

    // Is the instance assigned to a plugin?
    //
    void setAssigned(bool ass) { m_assigned = ass; }
    bool isAssigned() const { return m_assigned; }

    void setBypass(bool bypass) { m_bypass = bypass; }
    bool isBypassed() const { return m_bypass; }

    int getMappedId() const { return m_mappedId; }
    void setMappedId(int value) { m_mappedId = value; }

protected:

    int                                m_mappedId;
    std::string                        m_identifier;
    std::vector<PluginPortInstance*>   m_ports;
    unsigned int                       m_position;

    // Is the plugin actually assigned i.e. should we create
    // a matching instance at the sequencer?
    //
    bool                               m_assigned; 
    bool                               m_bypass;

};

}

#endif // _AUDIOPLUGININSTANCE_H_
