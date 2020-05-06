#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>

#include "../src/Streams.h"
#include "../src/Observer.h"

namespace rustla2 {

TEST(StreamsTest, TestWriteAPIJSON) {
    sqlite::database db(":memory:");

    auto streams = new Streams(db);
    auto status = Status(StatusCode::OK, "");
    auto chn = Channel::Create("test", "test", &status);
    auto stream = streams->Emplace(chn);

    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

    stream->WriteAPIJSON(&writer);

    auto output = buf.GetString();
    LOG(INFO) << output;
}

} // namespace rustla2

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
