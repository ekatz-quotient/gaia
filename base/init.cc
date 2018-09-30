// Copyright 2018, Beeri 15.  All rights reserved.
// Author: Roman Gershman (romange@gmail.com)
//

#include "base/init.h"

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "base/logging.h"
#include "base/walltime.h"

// #include <gperftools/malloc_extension.h>

namespace __internal__ {

ModuleInitializer::ModuleInitializer(VoidFunction ftor, bool is_ctor)
    : node_{ftor, global_list(), is_ctor} {
  global_list() = &node_;
}

auto ModuleInitializer::global_list() -> CtorNode*& {
  static CtorNode* my_global_list = nullptr;
  return my_global_list;
}

void ModuleInitializer::RunFtors(bool is_ctor) {
  CtorNode* node = global_list();
  while (node) {
    if (node->is_ctor == is_ctor)
      node->func();
    node = node->next;
  }
}

}  // namespace __internal__

#undef MainInitGuard

MainInitGuard::MainInitGuard(int* argc, char*** argv) {
  // MallocExtension::Initialize();
  google::ParseCommandLineFlags(argc, argv, true);
  google::InitGoogleLogging((*argv)[0]);

  absl::InitializeSymbolizer((*argv)[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);

  base::kProgramName = (*argv)[0];

#if defined NDEBUG
  LOG(INFO) << (*argv)[0] << " running in opt mode.";
#else
  LOG(INFO) << (*argv)[0] << " running in debug mode.";
#endif
  base::SetupJiffiesTimer();
  __internal__::ModuleInitializer::RunFtors(true);
}

MainInitGuard::~MainInitGuard() {
  __internal__::ModuleInitializer::RunFtors(false);
  base::DestroyJiffiesTimer();
  google::ShutdownGoogleLogging();
}
