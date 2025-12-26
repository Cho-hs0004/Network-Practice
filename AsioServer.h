#pragma once

using boost::asio::ip::tcp;

class Session;

class Server
{
public:
	Server(boost::asio::io_context& io, uint16_t port)
		: m_Acceptor(io, tcp::endpoint(tcp::v4(), port))
	{
		DoAccept();
	}

	void RecieveMsg(const std::string& msg);

private:
	void DoAccept();

	void BroadCastMsg();

	//荤侩磊 id
	UINT m_ID = 0;
	tcp::acceptor m_Acceptor;
	std::unordered_map<UINT, std::shared_ptr<Session>> UserMap; //技记 包府 甘
	std::string m_Msg;
};

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket, UINT id, Server* server)
		: m_Socket(std::move(socket)), m_ID(id), g_Server(server)
	{
	}

	void Start()
	{
		DoRead();
	}

	void DoWrite(const std::string& msg);

private:
	void DoRead();

	UINT m_ID;
	tcp::socket m_Socket;
	char m_Data[1024];
	Server* g_Server;
};