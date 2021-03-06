#pragma once

#include <thread>
#include <mutex>
#include <deque>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

namespace asio = boost::asio; 
typedef asio::ip::tcp tcp;
