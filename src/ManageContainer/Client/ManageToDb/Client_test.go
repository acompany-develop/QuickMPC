package mng2db

import (
	"testing"
)

/* InsertShares(string, []string, int32, string, string) error */
// シェアが送れるかTest
func TestInsertShares(t *testing.T) {
}

// 重複シェアを弾くかTest
func TestInsertSharesDuplicate(t *testing.T) {
}

/* DeleteShares([]string) error */
func TestDeleteShares(t *testing.T) {
}

/* GetSchema(string) ([]string, error) */
// Schemaが取得できるかTest
func TestGetSchema(t *testing.T) {
}

/* GetComputationResult(string) ([]*ComputationResult, error) */
// 計算結果が取得できるかTest
func TestGetComputationResult(t *testing.T) {
}

/* InsertModelParams(string, string, int32) error */
// モデルパラメータが送れるかTest
func TestInsertModelParams(t *testing.T) {
}

// 重複モデルパラメータを弾くかTest
func TestInsertModelParamsDuplicate(t *testing.T) {
}

/* GetDataList() (string, error) */
