#pragma once
// boost include header
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <deque>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//for logs
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
namespace logging = boost::log;
namespace src = boost::log::sources;
//extern src::logger lg;

//namespace set
using namespace boost;
using namespace boost::asio;
using boost::asio::ip::tcp;
/////////////////////////////////////////////////////////////////////
//for test
#include <boost/random.hpp>
//log test
#include <boost/lambda/lambda.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>

#include <boost/log/sources/record_ostream.hpp>


namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;