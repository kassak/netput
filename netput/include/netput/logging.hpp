#pragma once
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace np
{
   namespace log
   {
      using namespace boost::log;

      enum severity_type
      {
         trace, info, warning, critical
      };
      typedef
         sources::severity_channel_logger<severity_type, std::string>
         logger_t;
   }
}