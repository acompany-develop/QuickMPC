package utilsfortest

import (
	"fmt"
	"testing"

	m2db "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDbGate"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	"google.golang.org/grpc"
)

var conn *grpc.ClientConn

func init() {
	config, err := utils.GetConfig()
	DbgIp := config.Containers.Dbg
	if err != nil {
		AppLogger.Error(err)
	}
	conn, err = grpc.Dial(DbgIp.Host, grpc.WithInsecure())
	if err != nil {
		AppLogger.Fatalf("did not connect: %v", err)
	}
}

func DeleteId(t *testing.T, id string) {
	query1 := fmt.Sprintf("DELETE FROM `share` x "+
		"WHERE x.data_id='%s'"+
		"RETURNING x.*, meta().id ;", id)
	query2 := fmt.Sprintf("DELETE FROM `result` x "+
		"WHERE x.job_uuid='%s'"+
		"RETURNING x.*, meta().id ;", id)

	_, err := m2db.ExecuteQuery(conn, query1)
	if err != nil {
		t.Error("delete query faild: " + err.Error())
	}

	_, err = m2db.ExecuteQuery(conn, query2)
	if err != nil {
		t.Error("delete query faild: " + err.Error())
	}
}
