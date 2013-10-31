/*
 * Copyright 2013+ Ruslan Nigmatullin <euroelessar@yandex.ru>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COCAINE_SERVICE_NETWORKMANAGER_H
#define COCAINE_SERVICE_NETWORKMANAGER_H

#include "../http_response.hpp"
#include "../http_request.hpp"

#include "../logger.hpp"
#include "event_loop.hpp"
#include <memory>
#include <functional>
#include <map>

#include <boost/asio.hpp>
#include <boost/variant.hpp>

namespace ioremap {
namespace swarm {

class network_manager_private;
class url_fetcher_request_data;
class url_fetcher_response_data;
class base_stream;

class url_fetcher
{
public:
	url_fetcher(event_loop &loop, const ioremap::swarm::logger &logger);
	~url_fetcher();

	class request : public http_request
	{
	public:
		request();
		request(const boost::none_t &);
		request(request &&other);
		request(const request &other);
		~request();

		request &operator =(request &&other);
		request &operator =(const request &other);

		// Follow Location from 302 HTTP replies
		bool follow_location() const;
		void set_follow_location(bool follow_location);

		// Timeout in ms
		long timeout() const;
		void set_timeout(long timeout);

	private:
		std::unique_ptr<url_fetcher_request_data> m_data;
	};

	class response : public http_response
	{
	public:
		response();
		response(const boost::none_t &);
		response(response &&other);
		response(const response &other);
		~response();

		response &operator =(response &&other);
		response &operator =(const response &other);

		// Final URL from HTTP reply
		const swarm::url &url() const;
		void set_url(const swarm::url &url);
		void set_url(const std::string &url);

	private:
		std::unique_ptr<url_fetcher_response_data> m_data;
	};

	void set_limit(int active_connections);
	void set_logger(const swarm::logger &log);
	swarm::logger logger() const;

	void get(const std::shared_ptr<base_stream> &stream, url_fetcher::request &&request);
	void post(const std::shared_ptr<base_stream> &stream, url_fetcher::request &&request, std::string &&body);

private:
	url_fetcher(const url_fetcher &other);
	url_fetcher &operator =(const url_fetcher &other);

	network_manager_private *p;

	friend class network_manager_private;
};

class base_stream
{
public:
	virtual ~base_stream() {}

	virtual void on_headers(url_fetcher::response &&response) = 0;
	virtual void on_data(const boost::asio::const_buffer &data) = 0;
	virtual void on_close(const boost::system::error_code &error) = 0;
};

} // namespace service
} // namespace cocaine

#endif // COCAINE_SERVICE_NETWORKMANAGER_H