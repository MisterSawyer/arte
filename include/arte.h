#pragma once
#include <native.h>

#include <asio.hpp>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <format>
#include <string>
#include <unordered_set>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;

namespace this_coro = asio::this_coro;

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
#	define use_awaitable \
		asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#include <client/client.h>
#include <python/python.h>
#include <server/server.h>
