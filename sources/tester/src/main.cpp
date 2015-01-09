#include <iostream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <netput/config_parser.h>
#include <netput/loader.h>
#include <netput/netput.hpp>

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

   boost::shared_ptr<dyn_property_t> prop = boost::shared_ptr<dyn_property_t>(
      netput_parse_config_file(sink_config.c_str()),
      &dyn_prop_free
   );
   if(!prop)
   {
      std::cout << "Config load failed" << std::endl;
      return 1;
   }
   np::sink_wrapper_t sink;
   if(!netput_load_sink(prop.get(), &sink.desc()))
   {
      std::cout << "Sink load failed" << std::endl;
      return 1;
   }
   char c;
   bool ok = true;
   while(ok && (std::cin >> c))
   {
      switch(c)
      {
      case 'd':
         {
            size_t dev_id;
            if(std::cin >> dev_id)
               sink.set_device(dev_id);
         }
         break;
      case 'f':
         {
            sink.flush_device();
         }
         break;
      case 'k':
         {
            size_t key_id, val;
            if(std::cin >> key_id >> val)
               sink.set_key(key_id, val);
         }
         break;
      case 'q':
         ok = false;
         break;
      default:
         std::cout << "Skipping  command " << c << std::endl;
      }
   }
   netput_free_sink(&sink.desc());
}