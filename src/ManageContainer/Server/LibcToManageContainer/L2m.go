package l2mserver

import (
	"context"
	"encoding/json"
	"fmt"
	"strconv"

	"google.golang.org/grpc/reflection"

	m2c "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToComputationContainer"
	m2db "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToDb"
	m2m "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToManageContainer"
	m2t "github.com/acompany-develop/QuickMPC/src/ManageContainer/Client/ManageToTokenCA"
	. "github.com/acompany-develop/QuickMPC/src/ManageContainer/Log"
	common "github.com/acompany-develop/QuickMPC/src/ManageContainer/Server"
	utils "github.com/acompany-develop/QuickMPC/src/ManageContainer/Utils"
	pb "github.com/acompany-develop/QuickMPC/src/Proto/LibcToManageContainer"
	pb_types "github.com/acompany-develop/QuickMPC/src/Proto/common_types"
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
	token := in.GetToken()

	errToken := s.authorize(token, []string{"demo", "dep"})
	if errToken != nil {
		return &pb.SendSharesResponse{
			Message: errToken.Error(),
			IsOk:    false,
		}, errToken
	}
	if errToken != nil {
	}

	err := s.m2dbclient.InsertShares(dataID, schema, pieceID, shares, sent_at)
	s.m2mclient.Sync(fmt.Sprintf("%s%d", dataID, pieceID))

	if err != nil {
		AppLogger.Error(err)
		s.m2dbclient.DeleteShares([]string{dataID})
		s.m2mclient.DeleteShares(dataID)
		return &pb.SendSharesResponse{
			Message: err.Error(),
			IsOk:    false,
		}, err
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

	if err != nil {
		return &pb.ExecuteComputationResponse{
			Message: "Internal Server Error",
			IsOk:    false,
			JobUuid: jobUUID,
		}, err
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

	computationResults, err := s.m2dbclient.GetComputationResult(JobUUID)

	if err != nil {
		stream.Send(&pb.GetComputationResultResponse{
			Message: "Internal Server Error",
			IsOk:    false,
			Result:  "",
		})
		return err
	}

	for _, result := range computationResults {
		resultBytes, err := json.Marshal(result.Result)
		if err != nil {
			stream.Send(&pb.GetComputationResultResponse{
				Message: "Internal Server Error",
				IsOk:    false,
				Result:  "",
			})
			return err
		}

		stream.Send(&pb.GetComputationResultResponse{
			Message: "ok",
			IsOk:    true,
			Status:  pb_types.JobStatus(result.Status),
			Result:  string(resultBytes),
			PieceId: result.Meta.PieceID,
		})
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

	if err != nil {
		AppLogger.Error(err)
		return &pb.PredictResponse{
			Message: "Internal Server Error",
			IsOk:    false,
		}, err
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
