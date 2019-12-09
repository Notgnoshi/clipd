#pragma once
#include <zyre.h>

#include <iostream>
#include <string>
#include <variant>

namespace Clipd::Network::Messages
{
enum class MessageType
{
    Enter,   //!< A new peer has joined the network.
    Exit,    //!< A peer has explicitly left the network.
    Evasive, //!< A peer hasn't been heard from recently.
    Join,    //!< A peer has joined a group.
    Leave,   //!< A peer has left a group.
    Whisper, //!< A peer has messaged a particular node.
    Shout,   //!< A peer has broadcast a message to an entire group.
    Unknown, //!< Something else has happend?!
};

struct Enter
{
    std::string uuid;
    std::string name;
    std::string headers;
    std::string address;

    Enter( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
        headers = std::string( zmsg_popstr( msg ) );
        address = std::string( zmsg_popstr( msg ) );
    }
};

struct Exit
{
    std::string uuid;
    std::string name;

    Exit( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
    }
};

struct Evasive
{
    std::string uuid;
    std::string name;

    Evasive( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
    }
};

struct Join
{
    std::string uuid;
    std::string name;
    std::string groupname;

    Join( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
        groupname = std::string( zmsg_popstr( msg ) );
    }
};

struct Leave
{
    std::string uuid;
    std::string name;
    std::string groupname;

    Leave( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
        groupname = std::string( zmsg_popstr( msg ) );
    }
};

struct Whisper
{
    std::string uuid;
    std::string name;
    std::string message;

    Whisper( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
        message = std::string( zmsg_popstr( msg ) );
    }
};

struct Shout
{
    std::string uuid;
    std::string name;
    std::string groupname;
    std::string message;

    Shout( zmsg_t* msg )
    {
        uuid = std::string( zmsg_popstr( msg ) );
        name = std::string( zmsg_popstr( msg ) );
        groupname = std::string( zmsg_popstr( msg ) );
        message = std::string( zmsg_popstr( msg ) );
    }
};

//! @brief Convert a zframe_t to a string.
static std::string parseFrameStr( zframe_t* frame )
{
    return std::string( reinterpret_cast<char*>( zframe_data( frame ) ), zframe_size( frame ) );
}

/**
 * @brief Parse the first frame from the message to determine the Zyre message type.
 *
 * @param msg The message to parse as a Zyre message.
 * @return The type of the Zyre message
 */
MessageType parseMessageType( zmsg_t* msg )
{
    zframe_t* frame = zmsg_pop( msg );
    std::string type = parseFrameStr( frame );

    if( type == "ENTER" )
    {
        return MessageType::Enter;
    }
    else if( type == "EXIT" )
    {
        return MessageType::Exit;
    }
    else if( type == "EVASIVE" )
    {
        return MessageType::Evasive;
    }
    else if( type == "JOIN" )
    {
        return MessageType::Join;
    }
    else if( type == "LEAVE" )
    {
        return MessageType::Leave;
    }
    else if( type == "WHISPER" )
    {
        return MessageType::Whisper;
    }
    else if( type == "SHOUT" )
    {
        return MessageType::Shout;
    }

    return MessageType::Unknown;
}

std::ostream& operator<<( std::ostream& o, const Enter& msg )
{
    o << "Enter:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;
    o << "\theaders: " << msg.headers << std::endl;
    o << "\taddress: " << msg.address << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Exit& msg )
{
    o << "Exit:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Evasive& msg )
{
    o << "Evasive:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Join& msg )
{
    o << "Join:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;
    o << "\tgroupname: " << msg.groupname << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Leave& msg )
{
    o << "Leave:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;
    o << "\tgroupname: " << msg.groupname << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Whisper& msg )
{
    o << "Whisper:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;
    o << "\tmessage: " << msg.message << std::endl;

    return o;
}
std::ostream& operator<<( std::ostream& o, const Shout& msg )
{
    o << "Shout:" << std::endl;
    o << "\tuuid: " << msg.uuid << std::endl;
    o << "\tname: " << msg.name << std::endl;
    o << "\tgroupname: " << msg.groupname << std::endl;
    o << "\tmessage: " << msg.message << std::endl;

    return o;
}

} // namespace Clipd::Network::Messages
