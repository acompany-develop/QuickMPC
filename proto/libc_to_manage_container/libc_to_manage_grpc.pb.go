// Code generated by protoc-gen-go-grpc. DO NOT EDIT.
// versions:
// - protoc-gen-go-grpc v1.2.0
// - protoc             v3.19.1
// source: libc_to_manage.proto

package libc_to_manage_container

import (
	context "context"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
)

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
// Requires gRPC-Go v1.32.0 or later.
const _ = grpc.SupportPackageIsVersion7

// LibcToManageClient is the client API for LibcToManage service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://pkg.go.dev/google.golang.org/grpc/?tab=doc#ClientConn.NewStream.
type LibcToManageClient interface {
	SendShares(ctx context.Context, in *SendSharesRequest, opts ...grpc.CallOption) (*emptypb.Empty, error)
	DeleteShares(ctx context.Context, in *DeleteSharesRequest, opts ...grpc.CallOption) (*emptypb.Empty, error)
	GetSchema(ctx context.Context, in *GetSchemaRequest, opts ...grpc.CallOption) (*GetSchemaResponse, error)
	ExecuteComputation(ctx context.Context, in *ExecuteComputationRequest, opts ...grpc.CallOption) (*ExecuteComputationResponse, error)
	GetComputationResult(ctx context.Context, in *GetComputationResultRequest, opts ...grpc.CallOption) (LibcToManage_GetComputationResultClient, error)
	GetDataList(ctx context.Context, in *GetDataListRequest, opts ...grpc.CallOption) (*GetDataListResponse, error)
	GetElapsedTime(ctx context.Context, in *GetElapsedTimeRequest, opts ...grpc.CallOption) (*GetElapsedTimeResponse, error)
	GetJobErrorInfo(ctx context.Context, in *GetJobErrorInfoRequest, opts ...grpc.CallOption) (*GetJobErrorInfoResponse, error)
	AddShareDataFrame(ctx context.Context, in *AddShareDataFrameRequest, opts ...grpc.CallOption) (*AddShareDataFrameResponse, error)
}

type libcToManageClient struct {
	cc grpc.ClientConnInterface
}

func NewLibcToManageClient(cc grpc.ClientConnInterface) LibcToManageClient {
	return &libcToManageClient{cc}
}

func (c *libcToManageClient) SendShares(ctx context.Context, in *SendSharesRequest, opts ...grpc.CallOption) (*emptypb.Empty, error) {
	out := new(emptypb.Empty)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/SendShares", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) DeleteShares(ctx context.Context, in *DeleteSharesRequest, opts ...grpc.CallOption) (*emptypb.Empty, error) {
	out := new(emptypb.Empty)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/DeleteShares", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) GetSchema(ctx context.Context, in *GetSchemaRequest, opts ...grpc.CallOption) (*GetSchemaResponse, error) {
	out := new(GetSchemaResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/GetSchema", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) ExecuteComputation(ctx context.Context, in *ExecuteComputationRequest, opts ...grpc.CallOption) (*ExecuteComputationResponse, error) {
	out := new(ExecuteComputationResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/ExecuteComputation", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) GetComputationResult(ctx context.Context, in *GetComputationResultRequest, opts ...grpc.CallOption) (LibcToManage_GetComputationResultClient, error) {
	stream, err := c.cc.NewStream(ctx, &LibcToManage_ServiceDesc.Streams[0], "/libctomanage.LibcToManage/GetComputationResult", opts...)
	if err != nil {
		return nil, err
	}
	x := &libcToManageGetComputationResultClient{stream}
	if err := x.ClientStream.SendMsg(in); err != nil {
		return nil, err
	}
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	return x, nil
}

type LibcToManage_GetComputationResultClient interface {
	Recv() (*GetComputationResultResponse, error)
	grpc.ClientStream
}

type libcToManageGetComputationResultClient struct {
	grpc.ClientStream
}

func (x *libcToManageGetComputationResultClient) Recv() (*GetComputationResultResponse, error) {
	m := new(GetComputationResultResponse)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *libcToManageClient) GetDataList(ctx context.Context, in *GetDataListRequest, opts ...grpc.CallOption) (*GetDataListResponse, error) {
	out := new(GetDataListResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/GetDataList", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) GetElapsedTime(ctx context.Context, in *GetElapsedTimeRequest, opts ...grpc.CallOption) (*GetElapsedTimeResponse, error) {
	out := new(GetElapsedTimeResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/GetElapsedTime", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) GetJobErrorInfo(ctx context.Context, in *GetJobErrorInfoRequest, opts ...grpc.CallOption) (*GetJobErrorInfoResponse, error) {
	out := new(GetJobErrorInfoResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/GetJobErrorInfo", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *libcToManageClient) AddShareDataFrame(ctx context.Context, in *AddShareDataFrameRequest, opts ...grpc.CallOption) (*AddShareDataFrameResponse, error) {
	out := new(AddShareDataFrameResponse)
	err := c.cc.Invoke(ctx, "/libctomanage.LibcToManage/AddShareDataFrame", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// LibcToManageServer is the server API for LibcToManage service.
// All implementations must embed UnimplementedLibcToManageServer
// for forward compatibility
type LibcToManageServer interface {
	SendShares(context.Context, *SendSharesRequest) (*emptypb.Empty, error)
	DeleteShares(context.Context, *DeleteSharesRequest) (*emptypb.Empty, error)
	GetSchema(context.Context, *GetSchemaRequest) (*GetSchemaResponse, error)
	ExecuteComputation(context.Context, *ExecuteComputationRequest) (*ExecuteComputationResponse, error)
	GetComputationResult(*GetComputationResultRequest, LibcToManage_GetComputationResultServer) error
	GetDataList(context.Context, *GetDataListRequest) (*GetDataListResponse, error)
	GetElapsedTime(context.Context, *GetElapsedTimeRequest) (*GetElapsedTimeResponse, error)
	GetJobErrorInfo(context.Context, *GetJobErrorInfoRequest) (*GetJobErrorInfoResponse, error)
	AddShareDataFrame(context.Context, *AddShareDataFrameRequest) (*AddShareDataFrameResponse, error)
	mustEmbedUnimplementedLibcToManageServer()
}

// UnimplementedLibcToManageServer must be embedded to have forward compatible implementations.
type UnimplementedLibcToManageServer struct {
}

func (UnimplementedLibcToManageServer) SendShares(context.Context, *SendSharesRequest) (*emptypb.Empty, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SendShares not implemented")
}
func (UnimplementedLibcToManageServer) DeleteShares(context.Context, *DeleteSharesRequest) (*emptypb.Empty, error) {
	return nil, status.Errorf(codes.Unimplemented, "method DeleteShares not implemented")
}
func (UnimplementedLibcToManageServer) GetSchema(context.Context, *GetSchemaRequest) (*GetSchemaResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetSchema not implemented")
}
func (UnimplementedLibcToManageServer) ExecuteComputation(context.Context, *ExecuteComputationRequest) (*ExecuteComputationResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method ExecuteComputation not implemented")
}
func (UnimplementedLibcToManageServer) GetComputationResult(*GetComputationResultRequest, LibcToManage_GetComputationResultServer) error {
	return status.Errorf(codes.Unimplemented, "method GetComputationResult not implemented")
}
func (UnimplementedLibcToManageServer) GetDataList(context.Context, *GetDataListRequest) (*GetDataListResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetDataList not implemented")
}
func (UnimplementedLibcToManageServer) GetElapsedTime(context.Context, *GetElapsedTimeRequest) (*GetElapsedTimeResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetElapsedTime not implemented")
}
func (UnimplementedLibcToManageServer) GetJobErrorInfo(context.Context, *GetJobErrorInfoRequest) (*GetJobErrorInfoResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetJobErrorInfo not implemented")
}
func (UnimplementedLibcToManageServer) AddShareDataFrame(context.Context, *AddShareDataFrameRequest) (*AddShareDataFrameResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method AddShareDataFrame not implemented")
}
func (UnimplementedLibcToManageServer) mustEmbedUnimplementedLibcToManageServer() {}

// UnsafeLibcToManageServer may be embedded to opt out of forward compatibility for this service.
// Use of this interface is not recommended, as added methods to LibcToManageServer will
// result in compilation errors.
type UnsafeLibcToManageServer interface {
	mustEmbedUnimplementedLibcToManageServer()
}

func RegisterLibcToManageServer(s grpc.ServiceRegistrar, srv LibcToManageServer) {
	s.RegisterService(&LibcToManage_ServiceDesc, srv)
}

func _LibcToManage_SendShares_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(SendSharesRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).SendShares(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/SendShares",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).SendShares(ctx, req.(*SendSharesRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_DeleteShares_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(DeleteSharesRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).DeleteShares(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/DeleteShares",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).DeleteShares(ctx, req.(*DeleteSharesRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_GetSchema_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetSchemaRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).GetSchema(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/GetSchema",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).GetSchema(ctx, req.(*GetSchemaRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_ExecuteComputation_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(ExecuteComputationRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).ExecuteComputation(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/ExecuteComputation",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).ExecuteComputation(ctx, req.(*ExecuteComputationRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_GetComputationResult_Handler(srv interface{}, stream grpc.ServerStream) error {
	m := new(GetComputationResultRequest)
	if err := stream.RecvMsg(m); err != nil {
		return err
	}
	return srv.(LibcToManageServer).GetComputationResult(m, &libcToManageGetComputationResultServer{stream})
}

type LibcToManage_GetComputationResultServer interface {
	Send(*GetComputationResultResponse) error
	grpc.ServerStream
}

type libcToManageGetComputationResultServer struct {
	grpc.ServerStream
}

func (x *libcToManageGetComputationResultServer) Send(m *GetComputationResultResponse) error {
	return x.ServerStream.SendMsg(m)
}

func _LibcToManage_GetDataList_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetDataListRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).GetDataList(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/GetDataList",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).GetDataList(ctx, req.(*GetDataListRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_GetElapsedTime_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetElapsedTimeRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).GetElapsedTime(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/GetElapsedTime",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).GetElapsedTime(ctx, req.(*GetElapsedTimeRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_GetJobErrorInfo_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetJobErrorInfoRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).GetJobErrorInfo(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/GetJobErrorInfo",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).GetJobErrorInfo(ctx, req.(*GetJobErrorInfoRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _LibcToManage_AddShareDataFrame_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(AddShareDataFrameRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(LibcToManageServer).AddShareDataFrame(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/libctomanage.LibcToManage/AddShareDataFrame",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(LibcToManageServer).AddShareDataFrame(ctx, req.(*AddShareDataFrameRequest))
	}
	return interceptor(ctx, in, info, handler)
}

// LibcToManage_ServiceDesc is the grpc.ServiceDesc for LibcToManage service.
// It's only intended for direct use with grpc.RegisterService,
// and not to be introspected or modified (even as a copy)
var LibcToManage_ServiceDesc = grpc.ServiceDesc{
	ServiceName: "libctomanage.LibcToManage",
	HandlerType: (*LibcToManageServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "SendShares",
			Handler:    _LibcToManage_SendShares_Handler,
		},
		{
			MethodName: "DeleteShares",
			Handler:    _LibcToManage_DeleteShares_Handler,
		},
		{
			MethodName: "GetSchema",
			Handler:    _LibcToManage_GetSchema_Handler,
		},
		{
			MethodName: "ExecuteComputation",
			Handler:    _LibcToManage_ExecuteComputation_Handler,
		},
		{
			MethodName: "GetDataList",
			Handler:    _LibcToManage_GetDataList_Handler,
		},
		{
			MethodName: "GetElapsedTime",
			Handler:    _LibcToManage_GetElapsedTime_Handler,
		},
		{
			MethodName: "GetJobErrorInfo",
			Handler:    _LibcToManage_GetJobErrorInfo_Handler,
		},
		{
			MethodName: "AddShareDataFrame",
			Handler:    _LibcToManage_AddShareDataFrame_Handler,
		},
	},
	Streams: []grpc.StreamDesc{
		{
			StreamName:    "GetComputationResult",
			Handler:       _LibcToManage_GetComputationResult_Handler,
			ServerStreams: true,
		},
	},
	Metadata: "libc_to_manage.proto",
}
