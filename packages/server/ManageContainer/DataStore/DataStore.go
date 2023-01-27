package datastore

// PPコンテナ、Computationコンテナの状態  1 -> working, 2 -> not working
var StateOfPpContainer int32
var StateOfComputationContainer int32

// DBコンテナから取得した計算結果
var ComputationResults map[string]string = map[string]string{}
