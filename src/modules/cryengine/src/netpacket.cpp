// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#include <StdAfx.h>
#include "netpacket.h"
#include "global.h"

/* Simple packet
* // Create packet
* NetPacket packet(net_query);
* packet.WriteInt(net_query_auth);
* packet.WriteString("login");
* packet.WriteSrting("password");
* // Read packet
* NetPacket packet("recived_packet_data");
* packet.getType();
* packet.ReadInt();
* packet.ReadString();
* packet.ReadString();
*/

NetPacket::NetPacket(ENetPacketType type)
{
	m_data = "";
	m_separator = "|";
	m_type = net_Empty;
	m_MagicHeader = "";
	m_MagicFooter = "";

	// Only for reading
	bInitFromData = false;
	bIsGoodPacket = false;
	lastIndex = 0;
	//

	SetMagicHeader();
	SetPacketType(type);
}

NetPacket::NetPacket(const char * data)
{
	if (data)
	{
		m_data = data;
		m_type = net_Empty;
		m_separator = "|";
		m_MagicHeader = "";
		m_MagicFooter = "";

		bInitFromData = true;
		bIsGoodPacket = false;
		lastIndex = 0;

		ReadPacket();
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Empty packet");
		m_data = "";
		m_type = net_Empty;
		m_separator = "";
	}
}

void NetPacket::WriteString(std::string value)
{
	m_data = m_data + value + m_separator;
}

void NetPacket::WriteInt(int value)
{
	m_data = m_data + std::to_string(value) + m_separator;
}

void NetPacket::WriteBool(bool value)
{
	std::string m_value;
	value ? m_value = "true" : m_value = "false";

	m_data = m_data + m_value + m_separator;
}

void NetPacket::WriteFloat(float value)
{
	m_data = m_data + std::to_string(value) + m_separator;
}

void NetPacket::WriteDouble(double value)
{
	m_data = m_data + std::to_string(value) + m_separator;
}

const char* NetPacket::ReadString()
{
	if (bInitFromData && bIsGoodPacket)
	{
		if (m_packet.size() - 1 > lastIndex)
		{
			lastIndex++;
			return m_packet.at(lastIndex - 1).c_str();
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading string from packet. Last index wrong");
			return nullptr;
		}
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading string from packet. Bad packet.");
		return nullptr;
	}
}

int NetPacket::ReadInt()
{
	if (bInitFromData && bIsGoodPacket)
	{
		if (m_packet.size() - 1 > lastIndex)
		{
			lastIndex++;

			try
			{
				return std::stoi(m_packet.at(lastIndex - 1));
			}
			catch (std::invalid_argument &)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading int from packet. Can't convert string to int");
				return 0;
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading int from packet. Last index wrong");
			return 0;
		}
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading int from packet. Bad packet.");
		return 0;
	}
}

bool NetPacket::ReadBool()
{
	std::string m_value = ReadString();
	bool result;

	m_value == "true" ? result = true : result = false;
	return result;
}

float NetPacket::ReadFloat()
{
	if (bInitFromData && bIsGoodPacket)
	{
		if (m_packet.size() - 1 > lastIndex)
		{
			lastIndex++;

			try
			{
				return std::stof(m_packet.at(lastIndex - 1));
			}
			catch (std::invalid_argument &)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading float from packet. Can't convert string to float");
				return 0.0f;
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading float from packet. Last index wrong");
			return 0.0f;
		}
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading float from packet. Bad packet.");
		return 0.0f;
	}
}

double NetPacket::ReadDouble()
{
	if (bInitFromData && bIsGoodPacket)
	{
		if (m_packet.size() - 1 > lastIndex)
		{
			lastIndex++;

			try
			{
				return std::stod(m_packet.at(lastIndex - 1));
			}
			catch (std::invalid_argument &)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading double from packet. Can't convert string to double");
				return 0.0;
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading double from packet. Last index wrong");
			return 0.0;
		}
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading double from packet. Bad packet.");
		return 0.0;
	}
}

const char* NetPacket::toString()
{
	if (!bInitFromData)
	{
		SetMagicFooter();
		return m_data.c_str();
	}
	else
		return m_data.c_str();
}

ENetPacketType NetPacket::getType()
{
	return m_type;
}

void NetPacket::SetMagicHeader()
{
	int m_MagicValue = gModuleEnv->m_magic_key;
	char m_MagicKey[10] = "";
	itoa(m_MagicValue, m_MagicKey, 16);
	m_MagicHeader = "!0x" + std::string(m_MagicKey);

	WriteString(m_MagicHeader);
}

void NetPacket::SetPacketType(ENetPacketType type)
{
	WriteInt(type);
}

void NetPacket::SetMagicFooter()
{
	int m_MagicValue = gModuleEnv->m_magic_key;
	char m_MagicKey[10] = "";
	// Abracadabra
	itoa((m_MagicValue * 2.5) / 0.7 + 1945, m_MagicKey, 16);
	m_MagicFooter = "0x" + std::string(m_MagicKey) + "!";

	m_data = m_data + m_MagicFooter;
}

void NetPacket::ReadPacket()
{
	if (!m_data.empty())
	{
		// First generate header and footer
		int m_MagicValue = gModuleEnv->m_magic_key;
		char m_MagicKeyH[10] = "";
		itoa(m_MagicValue, m_MagicKeyH, 16);
		char m_MagicKeyF[10] = "";
		itoa((m_MagicValue * 2.5) / 0.7 + 1945, m_MagicKeyF, 16);
		m_MagicHeader = "!0x" + std::string(m_MagicKeyH);
		m_MagicFooter = "0x" + std::string(m_MagicKeyF) + "!";
		//

		m_packet = Split(m_data, m_separator.at(0));

		if (m_packet.size() >= 3)
		{
			std::string packet_header = m_packet.at(0);
			std::string packet_type = m_packet.at(1);
			std::string packet_footer = m_packet.at(m_packet.size() - 1);

			if (packet_header == m_MagicHeader && packet_footer == m_MagicFooter)
			{
				m_type = (ENetPacketType)std::stoi(packet_type);

				if (m_type > 0)
				{
					bIsGoodPacket = true;
					lastIndex = 2; // 0 - header, 1 - type, 2 - start data
				}
				else
				{
					CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading packet. Empty packet type");
					bIsGoodPacket = false;
				}
			}
			else
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading packet. Wrong magic key!");
				bIsGoodPacket = false;
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading packet. Packet soo small!");
			bIsGoodPacket = false;
		}
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, TITLE "Error reading packet. Packet empty!");
		bIsGoodPacket = false;
	}
}

std::vector<std::string> NetPacket::Split(const std::string & s, char delim)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	std::vector<std::string> m_vector;

	while (std::getline(ss, item, delim)) 
	{
		m_vector.push_back(item);
	}

	return m_vector;
}