#pragma once
#include <string>
#include <cstring>
#include <cstdlib>

namespace arte
{
    class Server
    {
        public:
            // ctor
            Server(asio::io_context & ioContext, short port)
            :   _ioContext(ioContext), _endpoint(tcp::v4(), port)
            {
                if(_running.test_and_set()){
                    std::puts("Server won't start. Weird running flag initialization.");
                    return;
                }
                co_spawn(ioContext, listen(), detached);
            }
            //dtor
            virtual ~Server()
            {
                Close();
            }
            //
            void Close()
            {
                _running.clear();
            }
    
        protected:
            awaitable<void> accept(tcp::socket socket)
            {
                const auto timeout = asio::chrono::seconds(180);
    
                std::puts(std::format("Connection with {} established", socket.remote_endpoint().address().to_string()).c_str());
                std::atomic_flag connected = ATOMIC_FLAG_INIT;
                connected.test_and_set();
    
                asio::steady_timer timer(socket.get_executor(), timeout);
                auto cancelToken = asio::bind_executor(socket.get_executor(), 
                    [&socket, &connected] (std::error_code ec) {
                        if(ec == asio::error::operation_aborted)return;
                        std::puts("Connection timer expired"); 
                        connected.clear(); socket.cancel();
                    }
                );
                const unsigned bufferLen = 1024;
                char data[bufferLen];
                try {
                    timer.expires_from_now(timeout);
                    timer.async_wait(cancelToken);
    
                    while (connected.test()) {
                    
                        std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
    
                        if(n > 0){
                            // reset connection timer
                            std::puts("refresh connection timer");
                            timer.expires_after(timeout);
                            timer.async_wait(cancelToken);
                        }
    
                        std::puts(std::format("echo read {} bytes from socket", n).c_str());

                        std::istringstream input(data);
                        std::vector<std::string> strs;

                        // parsing the headers
                        for (std::string line; std::getline(input, line);){
                            if (line.empty() || line == "\r") {
                                break; // end of headers reached
                            }
                            if (line.back() == '\r') {
                                line.resize(line.size()-1);
                            }

                            const char separator = ' ';
                            std::size_t pos = line.find(separator);
                            std::size_t initialPos = 0;
                            for(; pos != std::string::npos; pos = line.find( separator, initialPos )) {
                                strs.push_back( line.substr( initialPos, pos - initialPos ) );
                                initialPos = pos + 1;
                            }
                            // Add the last one
                            strs.push_back( line.substr( initialPos, std::min( pos, line.size() ) - initialPos + 1 ) );
                        }
                        if(strs.size() < 2)
                        {
                            std::puts("Invalid request");
                            continue;
                        }
                        std::puts(std::format("REQUEST {}", strs[0]).c_str());
                        std::puts(std::format("RESOURCE {}", strs[1]).c_str());

                        if(strs.at(1) == "/types")
                        {
                            std::string body = "[1, 2, 3, 4]";
                            std::string response = std::format(
                                "HTTP/1.1 200 OK\r\n"
                                "Server: Arte\r\n"
                                "Content-Type: application/json\r\n"
                                "Content-Length: {}\r\n"
                                "\r\n"
                                "{}",
                                body.size(), body
                            );

                            std::puts(std::format("Sending response :\n{}", response).c_str());
                            co_await asio::async_write(socket, asio::buffer(response.c_str(), response.length()), use_awaitable);
                        }

                    }
                } catch (std::exception& e) {
                    std::puts(std::format("echo Exception: {}", e.what()).c_str());
                }
                std::puts(std::format("Connection with {} ended", socket.remote_endpoint().address().to_string()).c_str());
                co_return;
            }
    
            awaitable<void> listen()
            {
                auto executor = co_await this_coro::executor;
    
                tcp::acceptor acceptor(executor, _endpoint);
                
                std::puts(std::format("server started listening on {} with port {}", _endpoint.address().to_string(), _endpoint.port()).c_str());
    
                while (_running.test()) {
                    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
                    _accepted.try_emplace(socket.remote_endpoint(), asio::make_strand(_ioContext));
                    co_spawn(executor, accept(std::move(socket)), detached);
                }
            }
        private:
            std::atomic_flag _running = ATOMIC_FLAG_INIT;
            asio::io_context & _ioContext;
            std::unordered_map<tcp::endpoint, asio::strand<asio::any_io_executor>> _accepted;
    
            const tcp::endpoint _endpoint; 
    };
}