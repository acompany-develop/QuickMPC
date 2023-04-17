package l2mserver

import (
	"context"
	"errors"
	"fmt"
	"strconv"

	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/reflection"
	"google.golang.org/grpc/status"

	m2c "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_computation_container"
	m2db "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_db"
	m2m "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_manage_container"
	m2t "github.com/acompany-develop/QuickMPC/packages/server/manage_container/client/manage_to_token_ca"
	. "github.com/acompany-develop/QuickMPC/packages/server/manage_container/log"
	common "github.com/acompany-develop/QuickMPC/packages/server/manage_container/server"
	utils "github.com/acompany-develop/QuickMPC/packages/server/manage_container/utils"
	pb "github.com/acompany-develop/QuickMPC/proto/libc_to_manage_container_v0"
	empty "github.com/golang/protobuf/ptypes/empty"
)

// ServerのInterface定義
type server_v0 struct {
	pb.UnimplementedLibcToManageServer
	m2dbclient m2db.M2DbClient
	m2cclient  m2c.M2CClient
	m2mclient  m2m.M2MClient
	m2tclient  m2t.M2TCAClient
}


func (s *server_v0) authorize(token string, stages []string) error {
	var errAll error = nil
	for _, stage := range stages {
		if stage == "demo" {
			err := s.m2tclient.AuthorizeDemo(token)
			if err == nil {
				return nil
			}
			errAll = err
		}
		if stage == "dep" {
			err := s.m2tclient.AuthorizeDep(token)
			if err == nil {
				return nil
			}
			errAll = err
		}
	}
	return errAll
}

// シェアをDBに送信
func (s *server_v0) SendShares(ctx context.Context, in *pb.SendSharesRequest) (*empty.Empty, error) {
	AppLogger.Info("Send Shares;")
	AppLogger.Info("dataID: " + in.GetDataId())
	AppLogger.Info("pieceID: " + strconv.Itoa(int(in.GetPieceId())))

	dataID := in.GetDataId()
	schema := in.GetSchema()
	pieceID := int32(in.GetPieceId())
	shares := in.GetShares()
	sent_at := in.GetSentAt()
	machingColumn := in.GetMatchingColumn()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	errInsert := s.m2dbclient.InsertShares(dataID, schema, pieceID, shares, sent_at, machingColumn)
	errSync := s.m2mclient.Sync(fmt.Sprintf("%s%d", dataID, pieceID))
	if errInsert != nil || errSync != nil {
		AppLogger.Error(errInsert)
		s.m2dbclient.DeleteShares([]string{dataID})
		s.m2mclient.DeleteShares(dataID)
		if errInsert != nil {
			return nil, errInsert
		}
		return nil, errSync
	}

	return &empty.Empty{}, nil
}

// シェア削除リクエストをDBに送信
func (s *server_v0) DeleteShares(ctx context.Context, in *pb.DeleteSharesRequest) (*empty.Empty, error) {
	AppLogger.Info("Delete Shares;")

	dataIDs := in.GetDataIds()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	err := s.m2dbclient.DeleteShares(dataIDs)
	if err != nil {
		AppLogger.Error(err)
		return nil, err
	}

	return &empty.Empty{}, nil
}

// DBからschemaを取得
func (s *server_v0) GetSchema(ctx context.Context, in *pb.GetSchemaRequest) (*pb.GetSchemaResponse, error) {
	AppLogger.Info("Get Schema;")
	AppLogger.Info("dataID:")
	AppLogger.Info(in.GetDataId())

	dataID := in.GetDataId()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	schema, err := s.m2dbclient.GetSchema(dataID)
	if err != nil {
		AppLogger.Error(err)
		return nil, errors.New("Internal server error")
	}

	return &pb.GetSchemaResponse{
		Schema:  schema,
	}, nil
}

// CCに計算リクエストを送信
func (s *server_v0) ExecuteComputation(ctx context.Context, in *pb.ExecuteComputationRequest) (*pb.ExecuteComputationResponse, error) {
	AppLogger.Info("Execute Computation;")
	AppLogger.Info("methodID: " + strconv.Itoa(int(in.GetMethodId())))
	AppLogger.Info("joinOrder:")
	AppLogger.Info(in.GetTable().GetDataIds(), in.GetTable().GetJoin(), in.GetTable().GetIndex())
	AppLogger.Info("Arg:")
	AppLogger.Info(in.GetArg().GetSrc(), in.GetArg().GetTarget())
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	dataIds := in.GetTable().GetDataIds()
	index := in.GetTable().GetIndex()
	for i := 0; i < len(dataIds); i++ {
		matchingColumn, err := s.m2dbclient.GetMatchingColumn(dataIds[i])
		if err != nil {
			AppLogger.Error(err)
			return nil, err
		}

		if matchingColumn != index[i] {
			errMessage := fmt.Sprintf("dataId:%s's matchingColumn must be %d, but value is %d", dataIds[i], matchingColumn, index[i])
			AppLogger.Error(errMessage)
			return nil, errors.New(errMessage)
		}
	}

	// JobUUIDを生成
	jobUUID, err := utils.CreateJobuuid()
	if err != nil {
		AppLogger.Error(err)
		return nil, err
	}
	AppLogger.Info("jobUUID: " + jobUUID)

	// 他パーティにstatus_RECEIVEDファイルを作成するようリクエストを送る
	err = s.m2mclient.CreateStatusFile(jobUUID)
	if err != nil {
		AppLogger.Error(err)
		s.m2mclient.DeleteStatusFile(jobUUID)
		return nil, err
	}
	// status_RECEIVEDファイルを作成する
	err = s.m2dbclient.CreateStatusFile(jobUUID)
	if err != nil {
		AppLogger.Error(err)
		s.m2dbclient.DeleteStatusFile(jobUUID)
		return nil, err
	}

	// 計算コンテナにリクエストを送信する
	out := utils.ConvertExecuteComputationRequest_v0(in, jobUUID)
	_, status, err := s.m2cclient.ExecuteComputation(out)

	if err != nil {
		AppLogger.Error(err)
		AppLogger.Error(status)
		// statusファイルを削除
		s.m2mclient.DeleteStatusFile(jobUUID)
		s.m2dbclient.DeleteStatusFile(jobUUID)
		return &pb.ExecuteComputationResponse{
			JobUuid: jobUUID,
		}, err
	}

	return &pb.ExecuteComputationResponse{
		JobUuid: jobUUID,
	}, nil
}

// DBから計算結果を得る
func (s *server_v0) GetComputationResult(in *pb.GetComputationResultRequest, stream pb.LibcToManage_GetComputationResultServer) error {
	AppLogger.Info("Get Computation Result;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())

	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return errToken
	}

	progress, err := s.m2cclient.CheckProgress(JobUUID)

	if err != nil {
		st, ok := status.FromError(err)
		if !ok {
			AppLogger.Errorf("GRPC Error : Code [%d], Message [%s]", st.Code(), st.Message())
		}
		// optional な情報のためロガーに残すのみでエラーを返さない
		logger_func := func(template string, args ...interface{}) {}
		switch st.Code() {
		case codes.NotFound:
			logger_func = AppLogger.Infof
		case
			codes.Internal,
			codes.Unavailable:
			logger_func = AppLogger.Warningf
		default:
			logger_func = AppLogger.Errorf
		}
		logger_func("gRPC CheckProgress method with JobUUID: [%s] returns error: Code [%s](%d), Message [%s]", JobUUID, st.Code().String(), st.Code(), st.Message())
	}

	computationResults, computationErrInfo, err := s.m2dbclient.GetComputationResult(JobUUID, []string{"dim1", "dim2", "schema"})

	if err != nil {
		return err
	}

	if computationErrInfo != nil {
		status, err := status.New(codes.Unknown, "computation result has error info").WithDetails(computationErrInfo)
		if err != nil {
			return err
		}
		return status.Err()
	}

	for _, result := range computationResults {
		response := pb.GetComputationResultResponse{
			Status:       computationResults[0].Status,
			Result:       result.Result,
			ColumnNumber: result.Meta.ColumnNumber,
			PieceId:      result.Meta.PieceID,
			Progress:     progress,
		}
		if result.Meta.ResultType == "dim1" {
			response.ResultType = &pb.GetComputationResultResponse_IsDim1{
				IsDim1: true,
			}
		} else if result.Meta.ResultType == "dim2" {
			response.ResultType = &pb.GetComputationResultResponse_IsDim2{
				IsDim2: true,
			}
		} else if result.Meta.ResultType == "schema" {
			response.ResultType = &pb.GetComputationResultResponse_IsSchema{
				IsSchema: true,
			}
		}
		stream.Send(&response)
	}

	return nil
}

func (s *server_v0) GetDataList(ctx context.Context, in *pb.GetDataListRequest) (*pb.GetDataListResponse, error) {
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	getDataList, err := s.m2dbclient.GetDataList()
	if err != nil {
		AppLogger.Error(err)
		return nil, err
	}

	return &pb.GetDataListResponse{
		Result: getDataList,
	}, nil
}

func (s *server_v0) GetElapsedTime(ctx context.Context, in *pb.GetElapsedTimeRequest) (*pb.GetElapsedTimeResponse, error) {
	AppLogger.Info("Get Elapsed time;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	elapsedTime, err := s.m2dbclient.GetElapsedTime(JobUUID)
	if err != nil {
		AppLogger.Error(err)
		return nil, err
	}

	return &pb.GetElapsedTimeResponse{
		ElapsedTime: elapsedTime,
	}, nil
}

func (s *server_v0) GetJobErrorInfo(ctx context.Context, in *pb.GetJobErrorInfoRequest) (*pb.GetJobErrorInfoResponse, error) {
	AppLogger.Info("Get Job Error Info;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return nil, errToken
	}

	errInfo, err := s.m2dbclient.GetJobErrorInfo(JobUUID)

	if err != nil {
		return nil, err
	}

	return &pb.GetJobErrorInfoResponse{
		JobErrorInfo: errInfo,
	}, nil
}

// LibtoMCサーバ起動
func RunServer() {
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("failed to parse config: %v", err)
	}
	ip := config.Containers.Manage
	lis, port := common.Listen(ip)

	s := common.NewServer()
	reflection.Register(s)

	pb.RegisterLibcToManageServer(s, &server{m2dbclient: m2db.Client{}, m2cclient: m2c.Client{}, m2mclient: m2m.Client{}, m2tclient: m2t.Client{}})

	AppLogger.Info("L2m server listening on", port)
	if err := s.Serve(lis); err != nil {
		AppLogger.Fatalf("failed to serve: %v", err)
	}
}
