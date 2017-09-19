#include <gtest/gtest.h>
#include <uWS/uWS.h>
#include <chrono>

#include "../src/HTTPRequest.h"
#include "../src/HTTPRouter.h"

namespace rustla2 {

namespace {

constexpr uWS::HttpMethod GET = uWS::HttpMethod::METHOD_GET;

std::function<void(const HTTPRouteHandler &handler)> GetHandler() {
  return [](const HTTPRouteHandler &handler) {
    uWS::HttpResponse *res = nullptr;
    HTTPRequest *req = nullptr;

    handler(res, req);
  };
}

}  // namespace

TEST(HTTPRouterTest, TestRoot) {
  rustla2::HTTPRouter router;
  uint64_t count = 0;

  router.Get("/", [&](uWS::HttpResponse *res, HTTPRequest *req) { ++count; });

  EXPECT_TRUE(router.Dispatch("/", GET, GetHandler()));
  EXPECT_FALSE(router.Dispatch("/test", GET, GetHandler()));

  EXPECT_EQ(count, 1);
}

TEST(HTTPRouterTest, TestWild) {
  rustla2::HTTPRouter router;
  uint64_t count = 0;

  router.Get("/some/wild/*",
             [&](uWS::HttpResponse *res, HTTPRequest *req) { ++count; });

  EXPECT_TRUE(router.Dispatch("/some/wild/test", GET, GetHandler()));
  EXPECT_FALSE(router.Dispatch("/some/wild/test/path", GET, GetHandler()));
  EXPECT_FALSE(router.Dispatch("/some/wild", GET, GetHandler()));

  EXPECT_EQ(count, 1);
}

TEST(HTTPRouterTest, TestWildSubpath) {
  rustla2::HTTPRouter router;
  uint64_t count = 0;

  router.Get("/some/wild/**",
             [&](uWS::HttpResponse *res, HTTPRequest *req) { ++count; });

  EXPECT_TRUE(router.Dispatch("/some/wild/test", GET, GetHandler()));
  EXPECT_TRUE(router.Dispatch("/some/wild/test/path", GET, GetHandler()));
  EXPECT_FALSE(router.Dispatch("/some/wild", GET, GetHandler()));

  EXPECT_EQ(count, 2);
}

}  // namespace rustla2

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
