#pragma once

#include "inmemtoxml.h"

//#include "levin/levin_server.h"

namespace epee
{

class activity_printer_base
{
public:
  activity_printer_base(){}
  virtual ~activity_printer_base(){}
};

template<class A>
class notify_activity_printer: public activity_printer_base
{
public:
  notify_activity_printer(int level, A& arg, bool is_notify_mode = true):m_ref_arg(arg), m_level(level), m_is_notify_mode(is_notify_mode)
  {
    m_command_name = typeid(m_ref_arg).name();
    m_command_name.erase(0, 7);
    m_command_name.erase(m_command_name.size()-10,  m_command_name.size()-1); 
    if(level == log_space::get_set_log_detalisation_level())
    {
      LOG_PRINT(m_command_name, level);
    }
    else if(level+1 == log_space::get_set_log_detalisation_level())
    {
      LOG_PRINT(" -->>" << m_command_name, level);
    }
    else if(level+2 == log_space::get_set_log_detalisation_level())
    {
      LOG_PRINT(" -->>" << m_command_name << "\n" << StorageNamed::xml::get_t_as_xml(m_ref_arg), level);
    }
  }

  virtual ~notify_activity_printer()
  {
    if(m_is_notify_mode)
    {
      if(m_level+1 == log_space::get_set_log_detalisation_level())
      {
        LOG_PRINT(" <<--" << m_command_name, m_level);
      }
    }
  }
protected:
  std::string m_command_name;
  A& m_ref_arg;
  int m_level;
  bool m_is_notify_mode;
};

template<class A, class R>
class command_activity_printer: public notify_activity_printer<A>
{
public:
  command_activity_printer(int level, A& arg, R& rsp):notify_activity_printer(level, arg, false), m_ref_rsp(rsp)
  {
  }

  virtual ~command_activity_printer()
  {
    if(m_level+1 == log_space::get_set_log_detalisation_level())
    {
      LOG_PRINT(" <<--" << m_command_name, m_level);
    }
    else if(m_level+2 == log_space::get_set_log_detalisation_level())
    {
      LOG_PRINT(" <<--" << m_command_name << "\n" << StorageNamed::trace_as_xml(m_ref_rsp), m_level);
    }
  }
private:
  R& m_ref_rsp;
};

template<class A, class R>
activity_printer_base* create_activity_printer(int level, A& arg, R& rsp)
{
  return new command_activity_printer<A, R>(level, arg, rsp);
}

template<class A>
activity_printer_base* create_activity_printer(int level, A& arg)
{
  return new notify_activity_printer<A>(level, arg);
}

}

#define PRINT_COMMAND_ACTIVITY(level) boost::shared_ptr<activity_printer_base> local_activity_printer(create_activity_printer(level, in_struct, out_struct));
#define PRINT_NOTIFY_ACTIVITY(level) boost::shared_ptr<activity_printer_base> local_activity_printer(create_activity_printer(level, in_struct));

#define PRINT_ACTIVITY(level) \
{std::string some_str = typeid(in_struct).name(); \
  some_str.erase(0, 7); \
  some_str.erase(some_str.size()-10,  some_str.size()-1); \
  LOG_PRINT(some_str, level);}

}

