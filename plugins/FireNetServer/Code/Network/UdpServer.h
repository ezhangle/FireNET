// Copyright (C) 2014-2017 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/FireNET/blob/master/LICENSE

#pragma once

#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <queue>
#include <map>

#include <FireNet>

#include "Network/UdpPacket.h"
#include "ReadQueue.h"

typedef boost::asio::io_service        BoostIO;
typedef boost::asio::ip::udp::socket   BoostUdpSocket;
typedef boost::asio::ip::udp::endpoint BoostUdpEndPoint;

struct SFireNetUdpServerClient
{
	uint32                          m_ID;
	BoostUdpEndPoint                m_EndPoint;

	SFireNetProfile*                pFireNetProfile;
	CReadQueue*                     pReader;
};

typedef std::map<uint32, SFireNetUdpServerClient> UdpClientList;
typedef UdpClientList::value_type UdpClient;

class CUdpServer
{
public:
	CUdpServer(BoostIO& io_service, const char* ip, short port);
	~CUdpServer();
public:
	void                                 Update();
	void                                 Close();
	short                                GetClientCount() { return m_Clients.size(); }
public:
	void                                 SendToClient(CUdpPacket &packet, uint32 clientID);
	void                                 SendToAll(CUdpPacket &packet);
private:	
	uint32                               GetOrCreateClientID(BoostUdpEndPoint endpoint);
	const SFireNetUdpServerClient*       GetClient(uint32 id);
	void                                 RemoveClient(uint32 id);
private:
	void                                 Do_Receive();
	void                                 Do_Send(BoostUdpEndPoint target);
private:
	void                                 On_RemoteError(const boost::system::error_code error_code, const BoostUdpEndPoint endPoint);
	void                                 On_ClientDisconnect(uint32 id);
private:
	void                                 MessageProcess(const char* data, uint32 id);
private:
	std::queue<CUdpPacket>               m_Queue;
private:
	BoostIO&                             m_IO_service;
	BoostUdpSocket                       m_UdpSocket;

	BoostUdpEndPoint                     m_RemoteEndPoint;
	uint32                               m_NextClientID;

	char                                 m_ReadBuffer[static_cast<int>(EFireNetUdpPackeMaxSize::SIZE)];
private:
	UdpClientList m_Clients;
};