#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

namespace asio = boost::asio; 

std::vector<char> vBuffer( 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, size_t length){
            if (!ec)
            {
                std::cout << "\n\nRead" << length << " bytes\n\n";
                for (int i = 0; i<length; ++i)
                    std::cout << vBuffer[i];
                GrabSomeData(socket);
            }
        }
    );
}

int main ()
{
    boost::system::error_code ec;

    // Create a "context" - essentially the platformspecific interface
    asio::io_context context; 

    asio::io_context::work idleWork(context);

    // Create a socket, the context will deliver the implimentation
    std::thread thContext = std::thread([&]() { context.run(); });

    // Get the address of somwhere we wish to connect to
    // 98.137.11.163:80 - yaho
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("98.137.11.163", ec), 80);
    
    // Tell socket to try and connect
    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if (ec)
        printf("%s%s", "Failed to connect\n", ec.message());
    else
        printf("%s", "Connected sucsess!\n");
    

    if(socket.is_open())
    {
        GrabSomeData(socket);   // v2
        std::string sRequest =
            "GET /index.html/1.1\r\n"
            "Host: examle.com\r\n"
            "Connection: close \r\n\r\n";

        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
        
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20000ms);

        /*socket.wait(socket.wait_read);

        size_t bytes = socket.available();
        std::cout << "Bytes Availible: " << bytes << std::endl;
        
        if (bytes > 0)
        {
            std::vector<char> vBuffer(bytes);
            socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
        
            for(auto c: vBuffer)
                std::cout << c;
        }*/

        

    }

    return 0;
}