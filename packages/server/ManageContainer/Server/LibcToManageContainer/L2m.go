package l2mserver

import (
	"context"
	"errors"
	"fmt"
	"strconv"

	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/reflection"
	"google.golang.org/grpc/status"

	m2c "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToComputationContainer"
	m2db "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDb"
	m2m "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToManageContainer"
	m2t "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToTokenCA"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	common "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/proto/LibcToManageContainer"
)

// ServerのInterface定義
type server struct {
	pb.UnimplementedLibcToManageServer
	m2dbclient m2db.M2DbClient
	m2cclient  m2c.M2CClient
	m2mclient  m2m.M2MClient
	m2tclient  m2t.M2TCAClient
}

func (s *server) authorize(token string, stages []string) error {
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
func (s *server) SendShares(ctx context.Context, in *pb.SendSharesRequest) (*pb.SendSharesResponse, error) {
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
		return &pb.SendSharesResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	errInsert := s.m2dbclient.InsertShares(dataID, schema, pieceID, shares, sent_at, machingColumn)
	errSync := s.m2mclient.Sync(fmt.Sprintf("%s%d", dataID, pieceID))
	if errInsert != nil || errSync != nil {
		AppLogger.Error(errInsert)
		s.m2dbclient.DeleteShares([]string{dataID})
		s.m2mclient.DeleteShares(dataID)
		if errInsert != nil {
			return &pb.SendSharesResponse{
				Message: errInsert.Error(),
				IsOk:    false,
			}, errInsert
		}
		return &pb.SendSharesResponse{
			Message: errSync.Error(),
			IsOk:    false,
		}, errSync
	}

	return &pb.SendSharesResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

// シェア削除リクエストをDBに送信
func (s *server) DeleteShares(ctx context.Context, in *pb.DeleteSharesRequest) (*pb.DeleteSharesResponse, error) {
	AppLogger.Info("Delete Shares;")

	dataIDs := in.GetDataIds()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.DeleteSharesResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	err := s.m2dbclient.DeleteShares(dataIDs)
	if err != nil {
		AppLogger.Error(err)
		return &pb.DeleteSharesResponse{
			Message: err.Error(),
			IsOk:    false,
		}, nil
	}

	return &pb.DeleteSharesResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

// DBからschemaを取得
func (s *server) GetSchema(ctx context.Context, in *pb.GetSchemaRequest) (*pb.GetSchemaResponse, error) {
	AppLogger.Info("Get Schema;")
	AppLogger.Info("dataID:")
	AppLogger.Info(in.GetDataId())

	dataID := in.GetDataId()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.GetSchemaResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	schema, err := s.m2dbclient.GetSchema(dataID)
	if err != nil {
		AppLogger.Error(err)
		return &pb.GetSchemaResponse{
			Message: "Internal server error",
			IsOk:    false,
			Schema:  schema,
		}, nil
	}

	return &pb.GetSchemaResponse{
		Message: "ok",
		IsOk:    true,
		Schema:  schema,
	}, nil
}

// CCに計算リクエストを送信
func (s *server) ExecuteComputation(ctx context.Context, in *pb.ExecuteComputationRequest) (*pb.ExecuteComputationResponse, error) {
	AppLogger.Info("Execute Computation;")
	AppLogger.Info("methodID: " + strconv.Itoa(int(in.GetMethodId())))
	AppLogger.Info("joinOrder:")
	AppLogger.Info(in.GetTable().GetDataIds(), in.GetTable().GetJoin(), in.GetTable().GetIndex())
	AppLogger.Info("Arg:")
	AppLogger.Info(in.GetArg().GetSrc(), in.GetArg().GetTarget())
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.ExecuteComputationResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	dataIds := in.GetTable().GetDataIds()
	index := in.GetTable().GetIndex()
	for i := 0; i < len(dataIds); i++ {
		matchingColumn, err := s.m2dbclient.GetMatchingColumn(dataIds[i])
		if err != nil {
			AppLogger.Error(err)
			return &pb.ExecuteComputationResponse{
				IsOk: false,
			}, err
		}

		if matchingColumn != index[i] {
			errMessage := fmt.Sprintf("dataId:%s's matchingColumn must be %d, but value is %d", dataIds[i], matchingColumn, index[i])
			AppLogger.Error(errMessage)
			return &pb.ExecuteComputationResponse{
				IsOk: false,
			}, errors.New(errMessage)
		}
	}

	// JobUUIDを生成
	jobUUID, err := utils.CreateJobuuid()
	if err != nil {
		AppLogger.Error(err)
		return &pb.ExecuteComputationResponse{
			Message: err.Error(),
			IsOk:    false,
		}, nil
	}
	AppLogger.Info("jobUUID: " + jobUUID)

	// 計算コンテナにリクエストを送信する
	out := utils.ConvertExecuteComputationRequest(in, jobUUID)
	message, status, err := s.m2cclient.ExecuteComputation(out)

	if err != nil {
		AppLogger.Error(err)
		AppLogger.Error(status)
		return &pb.ExecuteComputationResponse{
			Message: message,
			IsOk:    false,
			JobUuid: jobUUID,
		}, nil
	}

	return &pb.ExecuteComputationResponse{
		Message: "ok",
		IsOk:    true,
		JobUuid: jobUUID,
	}, nil
}

// DBから計算結果を得る
func (s *server) GetComputationResult(in *pb.GetComputationResultRequest, stream pb.LibcToManage_GetComputationResultServer) error {
	AppLogger.Info("Get Computation Result;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())

	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		stream.Send(&pb.GetComputationResultResponse{
			Message: errToken.Error(),
			IsOk:    false,
		})
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
		stream.Send(&pb.GetComputationResultResponse{
			Message: "Internal Server Error",
			IsOk:    false,
		})
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
			Message:      "ok",
			IsOk:         true,
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
func (s *server) SendModelParam(ctx context.Context, in *pb.SendModelParamRequest) (*pb.SendModelParamResponse, error) {
	AppLogger.Info("Send Model Parameters;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	AppLogger.Info("pieceId: " + strconv.Itoa(int(in.GetPieceId())))

	jobUUID := in.GetJobUuid()
	params := in.GetParams()
	token := in.GetToken()
	pieceId := in.GetPieceId()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.SendModelParamResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	err := s.m2dbclient.InsertModelParams(jobUUID, params, pieceId)
	if err != nil {
		AppLogger.Error(err)
		return &pb.SendModelParamResponse{
			Message: err.Error(),
			IsOk:    false,
		}, nil
	}

	return &pb.SendModelParamResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

// CCにモデル値予測リクエストを送信
func (s *server) Predict(ctx context.Context, in *pb.PredictRequest) (*pb.PredictResponse, error) {
	AppLogger.Info("Predict;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	AppLogger.Info("model param jobUUID: " + in.GetModelParamJobUuid())
	AppLogger.Info("methodID: " + strconv.Itoa(int(in.GetModelId())))
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.PredictResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}

	dataIds := in.GetTable().GetDataIds()
	index := in.GetTable().GetIndex()
	for i := 0; i < len(dataIds); i++ {
		matchingColumn, err := s.m2dbclient.GetMatchingColumn(dataIds[i])
		if err != nil {
			AppLogger.Error(err)
			return &pb.PredictResponse{
				IsOk: false,
			}, err
		}

		if matchingColumn != index[i] {
			errMessage := fmt.Sprintf("dataId:%s's matchingColumn must be %d, but value is %d", dataIds[i], matchingColumn, index[i])
			AppLogger.Error(errMessage)
			return &pb.PredictResponse{
				IsOk: false,
			}, errors.New(errMessage)
		}
	}

	out := utils.ConvertPredictRequest(in)
	message, status, err := s.m2cclient.Predict(out)

	if err != nil {
		AppLogger.Error(err)
		AppLogger.Error(status)
		return &pb.PredictResponse{
			Message: message,
			IsOk:    false,
		}, nil
	}

	return &pb.PredictResponse{
		Message: "ok",
		IsOk:    true,
	}, nil
}

func (s *server) GetDataList(ctx context.Context, in *pb.GetDataListRequest) (*pb.GetDataListResponse, error) {
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.GetDataListResponse{
			IsOk: false,
		}, errToken
	}
	getDataList, err := s.m2dbclient.GetDataList()
	if err != nil {
		AppLogger.Error(err)
		return &pb.GetDataListResponse{
			IsOk:   false,
			Result: "",
		}, nil
	}

	return &pb.GetDataListResponse{
		IsOk:   true,
		Result: getDataList,
	}, nil
}

func (s *server) GetElapsedTime(ctx context.Context, in *pb.GetElapsedTimeRequest) (*pb.GetElapsedTimeResponse, error) {
	AppLogger.Info("Get Elapsed time;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.GetElapsedTimeResponse{
			IsOk: false,
		}, errToken
	}
	elapsedTime, err := s.m2dbclient.GetElapsedTime(JobUUID)
	if err != nil {
		AppLogger.Error(err)
		return &pb.GetElapsedTimeResponse{
			IsOk: false,
		}, err
	}

	return &pb.GetElapsedTimeResponse{
		IsOk:        true,
		ElapsedTime: elapsedTime,
	}, nil
}

func (s *server) GetJobErrorInfo(ctx context.Context, in *pb.GetJobErrorInfoRequest) (*pb.GetJobErrorInfoResponse, error) {
	AppLogger.Info("Get Job Error Info;")
	AppLogger.Info("jobUUID: " + in.GetJobUuid())
	JobUUID := in.GetJobUuid()
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.GetJobErrorInfoResponse{
			IsOk: false,
		}, errToken
	}

	errInfo, err := s.m2dbclient.GetJobErrorInfo(JobUUID)

	if err != nil {
		return &pb.GetJobErrorInfoResponse{
			IsOk: false,
		}, err
	}

	return &pb.GetJobErrorInfoResponse{
		IsOk:         true,
		JobErrorInfo: errInfo,
	}, nil
}

// LibtoMCサーバ起動
func RunServer() {
	config, err := utils.GetConfig()
	if err != nil {
		AppLogger.Fatalf("failed to parse Config: %v", err)
	}
	ip := config.Containers.Manage
	lis, port := common.Listen(ip)

	s := common.NewServer()
	reflection.Register(s)

	pb.RegisterLibcToManageServer(s, &server{m2dbclient: m2db.Client{}, m2cclient: m2c.Client{}, m2mclient: m2m.Client{}, m2tclient: m2t.Client{}})

	AppLogger.Info("L2m Server listening on", port)
	if err := s.Serve(lis); err != nil {
		AppLogger.Fatalf("failed to serve: %v", err)
	}
}
