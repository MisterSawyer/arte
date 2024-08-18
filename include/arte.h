#pragma once
#include <native.h>

#include <cstdio>
#include <format>
#include <cstring>
#include <string>
#include <atomic>
#include <unordered_set>

#include <asio.hpp>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

namespace this_coro = asio::this_coro;

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#include <python/python.h>
#include <client/client.h>
#include <server/server.h>
