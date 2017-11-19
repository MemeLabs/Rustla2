#include <folly/experimental/FunctionScheduler.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <stdlib.h>
#include <uWS/uWS.h>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include "Config.h"
#include "DB.h"
#include "HTTPService.h"
#include "ServicePoller.h"
#include "WSService.h"

DEFINE_uint64(concurrency, 0, "Server thread count (defaults to core count)");

namespace rustla2 {

class Runner {
 public:
  Runner() : db_(new DB()) {}

  void Run() {
    ServicePoller service_poller(db_);
    folly::FunctionScheduler scheduler;
    scheduler.addFunction(
        [&]() { service_poller.Run(); },
        std::chrono::milliseconds(Config::Get().GetLivecheckInterval()),
        "ServicePoller");
    scheduler.start();

    auto concurrency = FLAGS_concurrency ? FLAGS_concurrency
                                         : std::thread::hardware_concurrency();
    LOG(INFO) << "starting " << concurrency << " server thread(s)";

    std::vector<std::thread *> threads(concurrency);
    std::transform(threads.begin(), threads.end(), threads.begin(),
                   [&](std::thread *t) { return CreateThread(); });

    for (const auto &thread : threads) {
      thread->join();
    }
  }

 private:
  std::thread *CreateThread() {
    return new std::thread([&]() {
      uWS::Hub hub;

      WSService ws_service(db_, &hub);
      HTTPService http_service(db_, &hub);

      if (!Listen(&hub)) {
        LOG(FATAL) << "unable to listen";
      }

      hub.run();
    });
  }

  bool Listen(uWS::Hub *hub) {
    auto port = Config::Get().GetPort();
    auto ssl_cert_path = Config::Get().GetSSLCertPath();
    auto ssl_key_path = Config::Get().GetSSLKeyPath();
    auto ssl_key_password = Config::Get().GetSSLKeyPassword();

    if (!ssl_cert_path.empty() && !ssl_key_path.empty()) {
      return hub->listen(
          port,
          uS::TLS::createContext(ssl_cert_path, ssl_key_path, ssl_key_password),
          uS::ListenOptions::REUSE_PORT);
    }

    return hub->listen(port, nullptr, uS::ListenOptions::REUSE_PORT);
  }

  std::shared_ptr<DB> db_;
};

}  // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);

  rustla2::Config::Get().Init(".env");

  rustla2::Runner runner;
  runner.Run();
}
