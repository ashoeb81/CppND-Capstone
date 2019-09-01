#include <iostream>
using namespace std;

#include <boost/program_options.hpp>
using namespace boost::program_options;

int main(int argc, char* argv[]) {
    string fileName;
    options_description desc("Allowed options");
    desc.add_options()
      ("file_name", value(&fileName), "pathname for output")
      ;
    
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
          
    std::cout << "Hello World!" << "\n";
    std::cout << "File Name: " << vm["file_name"].as<std::string>() << std::endl;
    return 0;
}