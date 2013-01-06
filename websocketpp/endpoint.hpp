/*
 * Copyright (c) 2012, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef WEBSOCKETPP_ENDPOINT_HPP
#define WEBSOCKETPP_ENDPOINT_HPP

#include <websocketpp/connection.hpp>

#include <iostream>
#include <set>

namespace websocketpp {

static const char user_agent[] = "WebSocket++/0.3.0dev";


// creates and manages connections
template <typename connection, typename config>
class endpoint 
  : public config::transport_type
{
public:
	// Import appropriate types from our helper class
	// See endpoint_types for more details.
	typedef endpoint<connection,config> type;
	
	typedef connection connection_type;
	typedef typename config::concurrency_type concurrency_type;
	typedef typename config::transport_type transport_type;
	
	typedef typename connection_type::ptr connection_ptr;
	typedef typename connection_type::weak_ptr connection_weak_ptr;
	typedef typename connection_type::handler handler_type;
	typedef typename handler_type::ptr handler_ptr;
	typedef typename connection_type::termination_handler termination_handler;
	
    typedef lib::shared_ptr<connection_weak_ptr> hdl_type;

	explicit endpoint(handler_ptr default_handler, bool is_server)
	  : m_default_handler(default_handler)
 	  , m_user_agent(::websocketpp::user_agent)
 	  , m_is_server(is_server)
	{
		std::cout << "endpoint constructor" << std::endl;
	}
	
	/// Returns the user agent string that this endpoint will use
	/**
	 * Returns the user agent string that this endpoint will use when creating
	 * new connections. 
	 *
	 * The default value for this version is WebSocket++/0.3.0dev
	 *
	 * @return A reference to the user agent string.
	 */ 
	const std::string& get_user_agent() const {
		return m_user_agent;
	}
	
	/// Sets the user agent string that this endpoint will use
	/**
	 * Sets the user agent string that this endpoint will use when creating
	 * new connections. Changing this value will only affect future connections.
	 *
	 * For best results set this before accepting or opening connections.
	 *
	 * The default value for this version is WebSocket++/0.3.0dev
	 *
	 * @return A reference to the user agent string.
	 */ 
	void set_user_agent(const std::string& ua) {
		m_user_agent = ua;
	}
	
	bool is_server() const {return m_is_server;}

    void set_open_handler(open_handler h) {
        m_open_handler = h;
    }

    void interrupt(connection_hdl hdl, lib::error_code & ec) {

        std::cout << "Interrupting connection " << &hdl << std::endl;
        
        /*lib::shared_ptr<void> s(hdl->lock());

        if (!s) {
            std::cout << "bad weak pointer in interrupt" << std::endl;
            return;
        }

        connection_ptr con = lib::static_pointer_cast<connection_type, void>(hdl)->lock();*/
        connection_ptr con = lib::static_pointer_cast<connection_type, void>(hdl.lock());

        if (!con) {
            ec = error::make_error_code(error::bad_connection);
            return;
        }

        std::cout << "CALL con->interrupt() HERE!!" << std::endl;
        ec = con->interrupt();

        //ec = lib::error_code();
    }

    void interrupt(connection_hdl hdl) {
        lib::error_code ec;
        interrupt(hdl,ec);
        if (ec) {
            throw ec;
        }
    }
protected:
	// Import appropriate internal types from our policy classes
	typedef typename concurrency_type::scoped_lock_type scoped_lock_type;
	typedef typename concurrency_type::mutex_type mutex_type;
	
	connection_ptr create_connection();
	void remove_connection(connection_ptr con);
    
    /// Retrieves a connection_ptr from a connection_hdl
    /**
     * @param hdl The connection handle to translate
     *
     * @return the connection_ptr. May be NULL if the handle was invalid.
     */
    connection_ptr get_con_from_hdl(connection_hdl hdl) {
        return lib::static_pointer_cast<connection_type,void>(hdl.lock());
    }

	// protected resources
	mutex_type	m_mutex;
private:
	// dynamic settings
	handler_ptr				    m_default_handler;
	std::string					m_user_agent;
	
    open_handler                m_open_handler;

	// endpoint resources
	std::set<connection_ptr>	m_connections;
	
	// static settings
	const bool					m_is_server;
	
	// endpoint state
	mutex_type                  m_state_lock;
};

} // namespace websocketpp

#include <websocketpp/impl/endpoint_impl.hpp>

#endif // WEBSOCKETPP_ENDPOINT_HPP
