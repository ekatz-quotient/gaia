// Copyright 2020, Beeri 15.  All rights reserved.
// Author: Roman Gershman (romange@gmail.com)
//
#pragma once

#include <boost/intrusive/slist.hpp>
#include <functional>
#include "util/uring/fiber_socket.h"

namespace util {
namespace uring {

class Proactor;
class AcceptServer;

class Connection {
  using connection_hook_t = ::boost::intrusive::slist_member_hook<
      ::boost::intrusive::link_mode<::boost::intrusive::auto_unlink>>;
  connection_hook_t hook_;

  void SetSocket(FiberSocket&& s) { socket_ = std::move(s); }
public:
  virtual ~Connection() {}

  using member_hook_t =
      ::boost::intrusive::member_hook<Connection, connection_hook_t, &Connection::hook_>;

protected:
  // The main loop for connection.
  virtual void HandleRequests(Proactor* proactor) = 0;

  FiberSocket socket_;
  friend class AcceptServer;
};

}  // namespace uring
}  // namespace util
