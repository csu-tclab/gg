/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef TRACED_PROCESS_HH
#define TRACED_PROCESS_HH

#include <sys/user.h>
#include <functional>
#include <unistd.h>
#include <cassert>
#include <csignal>
#include <map>
#include <functional>

#include "optional.hh"
#include "syscall.hh"
#include "invocation.hh"

class TraceControlBlock
{
private:
  bool detached_ { false };

public:
  pid_t pid;
  bool initialized { false };

  Optional<SystemCallInvocation> syscall_invocation {};

  void detach() { detached_ = true; }
  bool detached() const { return detached_; }

  TraceControlBlock( pid_t pid )
    : pid( pid )
  {}

  std::string to_string() const;
};

class TracedProcess
{
private:
  pid_t pid_;

  Optional<int> exit_status_;
  int graceful_termination_signal_;

  bool died_on_signal_;
  bool moved_away_;

  std::map<pid_t, TraceControlBlock> processes_;

  bool ptrace_syscall( pid_t & out_pid );

public:
  TracedProcess( std::function<int()> && child_procedure,
                 std::function<void()> && preparation_procedure = [](){},
                 const int termination_signal = SIGHUP );

  bool wait_for_syscall( std::function<void( TraceControlBlock & )> before_entry,
                         std::function<void( const TraceControlBlock & )> after_exit );

  void resume( void );

  template<typename T>
  static T get_syscall_arg( const pid_t pid, uint8_t argnum );

  template<typename T>
  static void set_syscall_arg( const pid_t pid, uint8_t argnum, const T & value );

  static user_regs_struct get_regs( const pid_t pid );

  pid_t pid( void ) const { assert( not moved_away_ ); return pid_; }
  Optional<int> exit_status() { return exit_status_; }

  ~TracedProcess();

  /* ban copying */
  TracedProcess( const TracedProcess & other ) = delete;
  TracedProcess & operator=( const TracedProcess & other ) = delete;

  /* allow move constructor */
  TracedProcess( TracedProcess && other );

  /* ... but not move assignment operator */
  TracedProcess & operator=( TracedProcess && other ) = delete;
};
#endif /* TRACED_PROCESS_HH */
