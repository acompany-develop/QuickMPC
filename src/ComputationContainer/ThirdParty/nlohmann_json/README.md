nlohmann::json
---

## 使い方
以下を参照
- [nlohmann/json 公式リポジトリ](https://github.com/nlohmann/json/blob/develop/README.md)
- [C++のjsonライブラリ決定版 nlohmnn-json](https://qiita.com/yohm/items/0f389ba5c5de4e2df9cf)


## サンプル
```C++
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
{
    nlohmann::json obj = {
        {"bazel", "https://bazel.build"},
        {"cmake", "https://cmake.org/"},
    };
    std::cout << obj.dump(4) << std::endl;
}

{
    std::ifstream ifs("/QuickMPC/Config/Config.json");
    nlohmann::json obj = nlohmann::json::parse(ifs);
    std::cout << obj.dump(4) << std::endl;
}
```