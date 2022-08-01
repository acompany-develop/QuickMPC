package dbg2db

import (
	"encoding/json"
	"errors"
	"log"
	"strings"
	"time"

	. "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Log"
	utils "github.com/acompany-develop/QuickMPC/src/DbContainer/DbGate/Utils"
	"github.com/couchbase/gocb/v2"
)

type QueryOpts struct {
	Host  string `required:"true"`
	Query string `required:"true"`
}

type QueriesOpts struct {
	Host    string   `required:"true"`
	Queries []string `required:"true"`
}

// 	cbのクラスタに接続したコクションインスタンスのポインタを持つマップ
// 	Key: string
//		HostName（Configと同じ名前）
//	Value: *gocb.Cluster
//		クラスターへのコネクションインスタンス
var clusterInstanceMap map[string]*gocb.Cluster

const connectionRetryLimit = 5

func connect(host string, username string, password string) (*gocb.Cluster, error) {
	var cluster *gocb.Cluster
	var err error
	for i := 0; i <= connectionRetryLimit; i++ {
		// Conect to Cluster
		cluster, err = gocb.Connect(
			host,
			gocb.ClusterOptions{
				Username: username,
				Password: password,
			},
		)
		if err != nil {
			continue
		}
		break
	}

	if err != nil {
		AppLogger.Errorf("%sへのgocb.Connetで%d回失敗: %v", host, connectionRetryLimit, err)
		return nil, err
	}

	return cluster, nil
}

// TODO: init()でpanicをおこなさなくて済むように分離を検討
func init() {
	clusterInstanceMap = map[string]*gocb.Cluster{}
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("Config読み込みエラー: %v", err)
	}

	sharedbCluster, err := connect(config.Containers.Sharedb.Url.Host, config.Containers.Sharedb.Username, config.Containers.Sharedb.Password)
	if err != nil {
		log.Fatalf("gocb.Connectに失敗: %v", err)
	}
	clusterInstanceMap["sharedb"] = sharedbCluster
	AppLogger.Info(clusterInstanceMap)
}

func CloseCluster() {
	for _, v := range clusterInstanceMap {
		v.Close(nil)
	}
}

// getCluster is clusterを返す関数
func getCluster(host string) (*gocb.Cluster, error) {
	LowerHost := strings.ToLower(host)
	cluster := clusterInstanceMap[LowerHost]
	// AppLogger.Info(LowerHost)
	if cluster == nil {
		return nil, errors.New("存在しないホストが指定された")
	}
	err := cluster.WaitUntilReady(5*time.Second, nil)
	if err != nil {
		return nil, err
	}
	return cluster, nil
}

// Clusterで指定されているDbへクエリを叩く関数
func executeQuery(cluster *gocb.Cluster, query string) (string, error) {

	// Queryが非同期に動くので，ScanConsistencyで同期的に実行しないと想定外挙動
	results, err := cluster.Query(query, &gocb.QueryOptions{
		ScanConsistency: gocb.QueryScanConsistencyRequestPlus,
	})

	if err != nil {
		return "", err
	}

	rowStrs := make([]string, 0)
	for results.Next() {
		var row interface{}
		err := results.Row(&row)
		if err != nil {
			return "", err
		}
		tmpBytes, err := json.Marshal(row)
		if err != nil {
			return "", err
		}
		tmpStr := string(tmpBytes)
		rowStrs = append(rowStrs, tmpStr)
	}

	return "[" + strings.Join(rowStrs[:], ",") + "]", nil
}

func ExecuteQuery(opts *QueryOpts) (string, error) {
	cluster, err := getCluster(opts.Host)
	if err != nil {
		log.Println(err)
		return "", err
	}

	result, err := executeQuery(cluster, opts.Query)

	if err != nil {
		return "", err
	}

	return result, nil
}
