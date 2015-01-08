#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, const char * argv[])
{
   std::string sink_config;
   po::options_description desc("General options");
   desc.add_options()
      ("help,h", "Show help")
      ("config,c", po::value<std::string>(&sink_config)->required(), "Sink configuration file")
   ;
   po::variables_map vm;
   po::store(po::parse_command_line(argc, argv, desc), vm);
   try
   {
      po::notify(vm);
   }
   catch(std::exception&)
   {
      std::cout << desc << "\n";
      return 1;
   }

   if(vm.count("help"))
   {
      std::cout << desc << "\n";
      return 1;
   }
}