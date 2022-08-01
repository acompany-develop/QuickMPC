# QuickMPC-Metrics
GithubActionsのログを加工してさまざまな指標を管理可能にするツール群

# 前準備
## GithubActionsのlogファイルダウンロード
それぞれ以下の箇所からダウンロード可能
### 特定のJobの結果logが欲しい場合
`GithubActionsのCI結果Summary画面 > 任意のJobの画面 > 画面右中央あたりの歯車アイコン > "View raw logs"`

### すべてのJobの結果logが欲しい場合
`GithubActionsのCI結果Summary画面 > 任意のJobの画面 > 画面右中央あたりの歯車アイコン > "Download log archive"`

![image](https://user-images.githubusercontent.com/33140349/147028682-5b6aaa54-3d0d-428f-a6ed-917df420788f.png)

# ga_prof
GithubActionsのProfilingツール
## 使い方
```sh
python3 ga_prof.py <log_file_path> --order_by <asc/desc>
```
- Required
  - `log_file_path`
- Optional
  - ` --order_by asc/desc`

## 出力例
```sh
 $ python3 ga_prof.py log/4405273696.log --order_by asc
 ...
 COPY Config/ComputationContainer/Comput..........  1.1s
 RUN wget -q -O cmake-linux.sh https://g..........  8.9s
 RUN wget -q -O cmake-linux.sh https://g..........  9.3s
 COPY Config/ComputationContainer/Comput..........  28.6s
 RUN apt-get update &&     apt-get insta..........  74.8s
 RUN apt-get update &&     apt-get insta..........  75.6s
 RUN cd / && git clone https://github.co..........  126.6s
 RUN cd / && git clone https://github.co..........  127.1s
 RUN bazel build //:all...........................  396.1s
 RUN bazel build //:all...........................  474.9s

                                      [TOTAL TIME]  1327.8s
```