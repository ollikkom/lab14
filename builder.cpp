#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace po = boost::program_options;

bp::child Make(int time = 0, std::string build="Debug")
{
  std::string cmd("cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=");
  cmd += "_install";
  cmd += " -DCMAKE_BUILD_TYPE=";
  cmd += build;
  std::cout << "Сommand = " << cmd << std::endl;
  bp::child с(cmd, bp::std_out > stdout);
  if (time) {
    if (!с.wait_for(std::chrono::seconds(time)));
      с.terminate();
  }
  else
    с.wait();
  return с;
}

bp::child Build(int time = 0)
{
  std::string cmd("cmake --build _builds");

  std::cout << "Сommand = " << cmd << std::endl;
  bp::child с(cmd, bp::std_out > stdout);
  if (time){
    if (!с.wait_for(std::chrono::seconds(time)));
      с.terminate();
  }
  else
    с.wait();
  return с;
}

bp::child Target(std::string inst)
{
  std::string cmd("cmake --build _builds");
  cmd += " --target ";
  cmd +=inst;

  std::cout << "Command = " << cmd << std::endl;
  bp::child c(cmd, bp::std_out > stdout);
  c.wait();
  return c;
}

int main(int argc, char *argv[])
{

    po::options_description description("Allowed options");
    description.add_options()
      ("help", "выводим вспомогательное сообщение")
      ("config", po::value<std::string>(), "указываем конфигурацию сборки (по умолчанию Debug)")
      ("install", "добавляем этап установки (в директорию _install)")
      ("pack", "добавляем этап упакови (в архив формата tar.gz)")
      ("timeout", po::value<int>(), "указываем время ожидания (в секундах)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);
    int total;

    if(vm.count("help"))
    {
      std::cout << description << std::endl;
      return 1;
    }
    else if (vm.count("config"))
    {
      std::string par(vm["config"].as<std::string>());
      total = Make(0, par).exit_code();
      if (!total)
        total = Build().exit_code();
    }
    else if (vm.count("install") && vm.count("pack"))
    {
      total = Make().exit_code();
      if (!total)
        total = Build().exit_code();
      if (!total)
        total = Target("install").exit_code();
        if (!total)
          total = Target("pack").exit_code();
    }
    else if (vm.count("install"))
    {
      total = Make().exit_code();
      if (!total)
        total = Build().exit_code();
      if (!total)
        total = Target("install").exit_code();
    }
    else if (vm.count("pack"))
    {
      total = Make().exit_code();
      if (!total)
        total = Build().exit_code();
      if (!total)
        total = Target("package").exit_code();
    }
    else if (vm.count("timeout"))
    {
      int tm = vm["timeout"].as<int>();
      total = Make(tm).exit_code();
      if (!total)
        total = Build(tm).exit_code();
    }
    else {
      total = Make().exit_code();
      if (!total)
        total = Build().exit_code();
    }
    return 0;
}
