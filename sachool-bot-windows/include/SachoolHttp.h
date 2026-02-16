#pragma once

#include <WinSock2.h>
#include <memory>
#include "../external-includes/httplib.h"

namespace Http {
	class SachoolHttp {
	private:
		std::unique_ptr<httplib::Server> m_Server;

	public:
		SachoolHttp();

		void startServer();
	};
}