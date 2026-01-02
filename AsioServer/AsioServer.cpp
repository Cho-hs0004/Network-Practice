#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "AsioServer.h"


void Server::DoAccept()
{
	{
		m_Acceptor.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					auto session = std::make_shared<Session>(std::move(socket), m_ID, this);
					session->Start();
					UserMap.emplace(m_ID, session);

					std::cout << "ID " << m_ID << " 접속" << std::endl;

					session->DoWrite("is " + std::to_string(m_ID));

					m_ID++;
				}

				DoAccept(); // 계속 accept
			}
		);
	}
}

void Server::RecieveMsg(const std::string& msg)
{
	m_Msg = msg;

	BroadCastMsg();
}

void Server::BroadCastMsg()
{
	for (auto& [id, user] : UserMap)
	{
		if (m_Msg.empty())
			return;

		//std::string id_msg = std::to_string(id) + " : " + m_Msg;

		user->DoWrite(m_Msg);
	}

	m_Msg.clear();
}

void Session::DoWrite(const std::string& msg)
{
	auto self = shared_from_this();
	boost::asio::async_write(
		m_Socket,
		boost::asio::buffer(msg.data(), msg.length()),
		[this, self](boost::system::error_code ec, std::size_t /*len*/)
		{
			if (!ec)
			{
				DoRead(); // 계속 읽기
			}
		}
	);
}

void Session::DoRead()
{
	auto self = shared_from_this();
	m_Socket.async_read_some(
		boost::asio::buffer(m_Data),
		[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				//서버에게 메시지가 들어왔다고 신호 + 메시지 전달
				std::string msg(m_Data, length);

				std::string id_msg = std::to_string(m_ID) + " : " + msg;

				g_Server->RecieveMsg(id_msg);

				std::cout << "Log (User " << m_ID << ") : " << msg << std::endl;
			}
			else
			{
				// Client disconnected
			}
		}
	);
}

int main()
{
	try
	{
		boost::asio::io_context io;
		Server server(io, 7777);

		std::cout << "[Async Echo] Server listening on port 7777...\n";
		io.run(); // 이벤트 루프 시작
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << "\n";
	}
}


