// Code generated by protoc-gen-go-grpc. DO NOT EDIT.
// versions:
// - protoc-gen-go-grpc v1.2.0
// - protoc             v3.19.1
// source: engine_to_bts.proto

package engine_to_bts

import (
	context "context"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
)

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
// Requires gRPC-Go v1.32.0 or later.
const _ = grpc.SupportPackageIsVersion7

// EngineToBtsClient is the client API for EngineToBts service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://pkg.go.dev/google.golang.org/grpc/?tab=doc#ClientConn.NewStream.
type EngineToBtsClient interface {
	GetTriples(ctx context.Context, in *GetRequest, opts ...grpc.CallOption) (*GetTriplesResponse, error)
	GetRandBits(ctx context.Context, in *GetRequest, opts ...grpc.CallOption) (*GetRandBitsResponse, error)
}

type engineToBtsClient struct {
	cc grpc.ClientConnInterface
}

func NewEngineToBtsClient(cc grpc.ClientConnInterface) EngineToBtsClient {
	return &engineToBtsClient{cc}
}

func (c *engineToBtsClient) GetTriples(ctx context.Context, in *GetRequest, opts ...grpc.CallOption) (*GetTriplesResponse, error) {
	out := new(GetTriplesResponse)
	err := c.cc.Invoke(ctx, "/enginetobts.EngineToBts/GetTriples", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *engineToBtsClient) GetRandBits(ctx context.Context, in *GetRequest, opts ...grpc.CallOption) (*GetRandBitsResponse, error) {
	out := new(GetRandBitsResponse)
	err := c.cc.Invoke(ctx, "/enginetobts.EngineToBts/GetRandBits", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// EngineToBtsServer is the server API for EngineToBts service.
// All implementations must embed UnimplementedEngineToBtsServer
// for forward compatibility
type EngineToBtsServer interface {
	GetTriples(context.Context, *GetRequest) (*GetTriplesResponse, error)
	GetRandBits(context.Context, *GetRequest) (*GetRandBitsResponse, error)
	mustEmbedUnimplementedEngineToBtsServer()
}

// UnimplementedEngineToBtsServer must be embedded to have forward compatible implementations.
type UnimplementedEngineToBtsServer struct {
}

func (UnimplementedEngineToBtsServer) GetTriples(context.Context, *GetRequest) (*GetTriplesResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetTriples not implemented")
}
func (UnimplementedEngineToBtsServer) GetRandBits(context.Context, *GetRequest) (*GetRandBitsResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetRandBits not implemented")
}
func (UnimplementedEngineToBtsServer) mustEmbedUnimplementedEngineToBtsServer() {}

// UnsafeEngineToBtsServer may be embedded to opt out of forward compatibility for this service.
// Use of this interface is not recommended, as added methods to EngineToBtsServer will
// result in compilation errors.
type UnsafeEngineToBtsServer interface {
	mustEmbedUnimplementedEngineToBtsServer()
}

func RegisterEngineToBtsServer(s grpc.ServiceRegistrar, srv EngineToBtsServer) {
	s.RegisterService(&EngineToBts_ServiceDesc, srv)
}

func _EngineToBts_GetTriples_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(EngineToBtsServer).GetTriples(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/enginetobts.EngineToBts/GetTriples",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(EngineToBtsServer).GetTriples(ctx, req.(*GetRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _EngineToBts_GetRandBits_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(EngineToBtsServer).GetRandBits(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/enginetobts.EngineToBts/GetRandBits",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(EngineToBtsServer).GetRandBits(ctx, req.(*GetRequest))
	}
	return interceptor(ctx, in, info, handler)
}

// EngineToBts_ServiceDesc is the grpc.ServiceDesc for EngineToBts service.
// It's only intended for direct use with grpc.RegisterService,
// and not to be introspected or modified (even as a copy)
var EngineToBts_ServiceDesc = grpc.ServiceDesc{
	ServiceName: "enginetobts.EngineToBts",
	HandlerType: (*EngineToBtsServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "GetTriples",
			Handler:    _EngineToBts_GetTriples_Handler,
		},
		{
			MethodName: "GetRandBits",
			Handler:    _EngineToBts_GetRandBits_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "engine_to_bts.proto",
}
