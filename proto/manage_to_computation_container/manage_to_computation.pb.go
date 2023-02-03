// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.26.0
// 	protoc        v3.19.1
// source: manage_to_computation.proto

package manage_to_computation_container

import (
	common_types "github.com/acompany-develop/QuickMPC/proto/common_types"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
	reflect "reflect"
	sync "sync"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

//*
// the message of ExecuteComputationRequest
type JoinOrder struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	DataIds []string `protobuf:"bytes,1,rep,name=dataIds,proto3" json:"dataIds,omitempty"`
	Join    []int32  `protobuf:"varint,2,rep,packed,name=join,proto3" json:"join,omitempty"`
	Index   []int32  `protobuf:"varint,3,rep,packed,name=index,proto3" json:"index,omitempty"`
}

func (x *JoinOrder) Reset() {
	*x = JoinOrder{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_computation_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *JoinOrder) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JoinOrder) ProtoMessage() {}

func (x *JoinOrder) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_computation_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JoinOrder.ProtoReflect.Descriptor instead.
func (*JoinOrder) Descriptor() ([]byte, []int) {
	return file_manage_to_computation_proto_rawDescGZIP(), []int{0}
}

func (x *JoinOrder) GetDataIds() []string {
	if x != nil {
		return x.DataIds
	}
	return nil
}

func (x *JoinOrder) GetJoin() []int32 {
	if x != nil {
		return x.Join
	}
	return nil
}

func (x *JoinOrder) GetIndex() []int32 {
	if x != nil {
		return x.Index
	}
	return nil
}

type Input struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Src    []int32 `protobuf:"varint,1,rep,packed,name=src,proto3" json:"src,omitempty"`
	Target []int32 `protobuf:"varint,2,rep,packed,name=target,proto3" json:"target,omitempty"`
}

func (x *Input) Reset() {
	*x = Input{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_computation_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Input) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Input) ProtoMessage() {}

func (x *Input) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_computation_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Input.ProtoReflect.Descriptor instead.
func (*Input) Descriptor() ([]byte, []int) {
	return file_manage_to_computation_proto_rawDescGZIP(), []int{1}
}

func (x *Input) GetSrc() []int32 {
	if x != nil {
		return x.Src
	}
	return nil
}

func (x *Input) GetTarget() []int32 {
	if x != nil {
		return x.Target
	}
	return nil
}

type ExecuteComputationRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	MethodId common_types.ComputationMethod `protobuf:"varint,1,opt,name=method_id,json=methodId,proto3,enum=pb_common_types.ComputationMethod" json:"method_id,omitempty"` // the methodID of this request
	JobUuid  string                         `protobuf:"bytes,2,opt,name=job_uuid,json=jobUuid,proto3" json:"job_uuid,omitempty"`                                            // the jobUUID of this request
	Table    *JoinOrder                     `protobuf:"bytes,3,opt,name=table,proto3" json:"table,omitempty"`
	Arg      *Input                         `protobuf:"bytes,4,opt,name=arg,proto3" json:"arg,omitempty"`
}

func (x *ExecuteComputationRequest) Reset() {
	*x = ExecuteComputationRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_computation_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *ExecuteComputationRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*ExecuteComputationRequest) ProtoMessage() {}

func (x *ExecuteComputationRequest) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_computation_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use ExecuteComputationRequest.ProtoReflect.Descriptor instead.
func (*ExecuteComputationRequest) Descriptor() ([]byte, []int) {
	return file_manage_to_computation_proto_rawDescGZIP(), []int{2}
}

func (x *ExecuteComputationRequest) GetMethodId() common_types.ComputationMethod {
	if x != nil {
		return x.MethodId
	}
	return common_types.ComputationMethod_COMPUTATION_METHOD_UNSPECIFIED
}

func (x *ExecuteComputationRequest) GetJobUuid() string {
	if x != nil {
		return x.JobUuid
	}
	return ""
}

func (x *ExecuteComputationRequest) GetTable() *JoinOrder {
	if x != nil {
		return x.Table
	}
	return nil
}

func (x *ExecuteComputationRequest) GetArg() *Input {
	if x != nil {
		return x.Arg
	}
	return nil
}

//*
// the message of CheckStateResponse
type CheckStateResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	State int32 `protobuf:"varint,1,opt,name=state,proto3" json:"state,omitempty"` // the state of computation container(1:working)
}

func (x *CheckStateResponse) Reset() {
	*x = CheckStateResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_computation_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CheckStateResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CheckStateResponse) ProtoMessage() {}

func (x *CheckStateResponse) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_computation_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CheckStateResponse.ProtoReflect.Descriptor instead.
func (*CheckStateResponse) Descriptor() ([]byte, []int) {
	return file_manage_to_computation_proto_rawDescGZIP(), []int{3}
}

func (x *CheckStateResponse) GetState() int32 {
	if x != nil {
		return x.State
	}
	return 0
}

type CheckProgressRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	JobUuid string `protobuf:"bytes,1,opt,name=job_uuid,json=jobUuid,proto3" json:"job_uuid,omitempty"`
}

func (x *CheckProgressRequest) Reset() {
	*x = CheckProgressRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_computation_proto_msgTypes[4]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CheckProgressRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CheckProgressRequest) ProtoMessage() {}

func (x *CheckProgressRequest) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_computation_proto_msgTypes[4]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CheckProgressRequest.ProtoReflect.Descriptor instead.
func (*CheckProgressRequest) Descriptor() ([]byte, []int) {
	return file_manage_to_computation_proto_rawDescGZIP(), []int{4}
}

func (x *CheckProgressRequest) GetJobUuid() string {
	if x != nil {
		return x.JobUuid
	}
	return ""
}

var File_manage_to_computation_proto protoreflect.FileDescriptor

var file_manage_to_computation_proto_rawDesc = []byte{
	0x0a, 0x1b, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x5f, 0x74, 0x6f, 0x5f, 0x63, 0x6f, 0x6d, 0x70,
	0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x13, 0x6d,
	0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69,
	0x6f, 0x6e, 0x1a, 0x1b, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f, 0x70, 0x72, 0x6f, 0x74, 0x6f,
	0x62, 0x75, 0x66, 0x2f, 0x65, 0x6d, 0x70, 0x74, 0x79, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a,
	0x1f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2f, 0x63, 0x6f,
	0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f,
	0x22, 0x4f, 0x0a, 0x09, 0x4a, 0x6f, 0x69, 0x6e, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x12, 0x18, 0x0a,
	0x07, 0x64, 0x61, 0x74, 0x61, 0x49, 0x64, 0x73, 0x18, 0x01, 0x20, 0x03, 0x28, 0x09, 0x52, 0x07,
	0x64, 0x61, 0x74, 0x61, 0x49, 0x64, 0x73, 0x12, 0x12, 0x0a, 0x04, 0x6a, 0x6f, 0x69, 0x6e, 0x18,
	0x02, 0x20, 0x03, 0x28, 0x05, 0x52, 0x04, 0x6a, 0x6f, 0x69, 0x6e, 0x12, 0x14, 0x0a, 0x05, 0x69,
	0x6e, 0x64, 0x65, 0x78, 0x18, 0x03, 0x20, 0x03, 0x28, 0x05, 0x52, 0x05, 0x69, 0x6e, 0x64, 0x65,
	0x78, 0x22, 0x31, 0x0a, 0x05, 0x49, 0x6e, 0x70, 0x75, 0x74, 0x12, 0x10, 0x0a, 0x03, 0x73, 0x72,
	0x63, 0x18, 0x01, 0x20, 0x03, 0x28, 0x05, 0x52, 0x03, 0x73, 0x72, 0x63, 0x12, 0x16, 0x0a, 0x06,
	0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x18, 0x02, 0x20, 0x03, 0x28, 0x05, 0x52, 0x06, 0x74, 0x61,
	0x72, 0x67, 0x65, 0x74, 0x22, 0xdb, 0x01, 0x0a, 0x19, 0x45, 0x78, 0x65, 0x63, 0x75, 0x74, 0x65,
	0x43, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x52, 0x65, 0x71, 0x75, 0x65,
	0x73, 0x74, 0x12, 0x3f, 0x0a, 0x09, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x5f, 0x69, 0x64, 0x18,
	0x01, 0x20, 0x01, 0x28, 0x0e, 0x32, 0x22, 0x2e, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f,
	0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x43, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74,
	0x69, 0x6f, 0x6e, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x52, 0x08, 0x6d, 0x65, 0x74, 0x68, 0x6f,
	0x64, 0x49, 0x64, 0x12, 0x19, 0x0a, 0x08, 0x6a, 0x6f, 0x62, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18,
	0x02, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x6a, 0x6f, 0x62, 0x55, 0x75, 0x69, 0x64, 0x12, 0x34,
	0x0a, 0x05, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1e, 0x2e,
	0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74,
	0x69, 0x6f, 0x6e, 0x2e, 0x4a, 0x6f, 0x69, 0x6e, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x52, 0x05, 0x74,
	0x61, 0x62, 0x6c, 0x65, 0x12, 0x2c, 0x0a, 0x03, 0x61, 0x72, 0x67, 0x18, 0x04, 0x20, 0x01, 0x28,
	0x0b, 0x32, 0x1a, 0x2e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x63, 0x6f, 0x6d, 0x70,
	0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x49, 0x6e, 0x70, 0x75, 0x74, 0x52, 0x03, 0x61,
	0x72, 0x67, 0x22, 0x2a, 0x0a, 0x12, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x53, 0x74, 0x61, 0x74, 0x65,
	0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x14, 0x0a, 0x05, 0x73, 0x74, 0x61, 0x74,
	0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x05, 0x52, 0x05, 0x73, 0x74, 0x61, 0x74, 0x65, 0x22, 0x31,
	0x0a, 0x14, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x52,
	0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x19, 0x0a, 0x08, 0x6a, 0x6f, 0x62, 0x5f, 0x75, 0x75,
	0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x6a, 0x6f, 0x62, 0x55, 0x75, 0x69,
	0x64, 0x32, 0xd1, 0x01, 0x0a, 0x13, 0x4d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x54, 0x6f, 0x43, 0x6f,
	0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x12, 0x5e, 0x0a, 0x12, 0x45, 0x78, 0x65,
	0x63, 0x75, 0x74, 0x65, 0x43, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x12,
	0x2e, 0x2e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74,
	0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x45, 0x78, 0x65, 0x63, 0x75, 0x74, 0x65, 0x43, 0x6f, 0x6d,
	0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a,
	0x16, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x62, 0x75,
	0x66, 0x2e, 0x45, 0x6d, 0x70, 0x74, 0x79, 0x22, 0x00, 0x12, 0x5a, 0x0a, 0x0d, 0x43, 0x68, 0x65,
	0x63, 0x6b, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x12, 0x29, 0x2e, 0x6d, 0x61, 0x6e,
	0x61, 0x67, 0x65, 0x74, 0x6f, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e,
	0x2e, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x52, 0x65,
	0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x1c, 0x2e, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f,
	0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x4a, 0x6f, 0x62, 0x50, 0x72, 0x6f, 0x67, 0x72,
	0x65, 0x73, 0x73, 0x22, 0x00, 0x42, 0x4d, 0x5a, 0x4b, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2e,
	0x63, 0x6f, 0x6d, 0x2f, 0x61, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x2d, 0x64, 0x65, 0x76,
	0x65, 0x6c, 0x6f, 0x70, 0x2f, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x4d, 0x50, 0x43, 0x2f, 0x73, 0x72,
	0x63, 0x2f, 0x50, 0x72, 0x6f, 0x74, 0x6f, 0x2f, 0x4d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x54, 0x6f,
	0x43, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x43, 0x6f, 0x6e, 0x74, 0x61,
	0x69, 0x6e, 0x65, 0x72, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_manage_to_computation_proto_rawDescOnce sync.Once
	file_manage_to_computation_proto_rawDescData = file_manage_to_computation_proto_rawDesc
)

func file_manage_to_computation_proto_rawDescGZIP() []byte {
	file_manage_to_computation_proto_rawDescOnce.Do(func() {
		file_manage_to_computation_proto_rawDescData = protoimpl.X.CompressGZIP(file_manage_to_computation_proto_rawDescData)
	})
	return file_manage_to_computation_proto_rawDescData
}

var file_manage_to_computation_proto_msgTypes = make([]protoimpl.MessageInfo, 5)
var file_manage_to_computation_proto_goTypes = []interface{}{
	(*JoinOrder)(nil),                   // 0: managetocomputation.JoinOrder
	(*Input)(nil),                       // 1: managetocomputation.Input
	(*ExecuteComputationRequest)(nil),   // 2: managetocomputation.ExecuteComputationRequest
	(*CheckStateResponse)(nil),          // 3: managetocomputation.CheckStateResponse
	(*CheckProgressRequest)(nil),        // 4: managetocomputation.CheckProgressRequest
	(common_types.ComputationMethod)(0), // 5: pb_common_types.ComputationMethod
	(*emptypb.Empty)(nil),               // 6: google.protobuf.Empty
	(*common_types.JobProgress)(nil),    // 7: pb_common_types.JobProgress
}
var file_manage_to_computation_proto_depIdxs = []int32{
	5, // 0: managetocomputation.ExecuteComputationRequest.method_id:type_name -> pb_common_types.ComputationMethod
	0, // 1: managetocomputation.ExecuteComputationRequest.table:type_name -> managetocomputation.JoinOrder
	1, // 2: managetocomputation.ExecuteComputationRequest.arg:type_name -> managetocomputation.Input
	2, // 3: managetocomputation.ManageToComputation.ExecuteComputation:input_type -> managetocomputation.ExecuteComputationRequest
	4, // 4: managetocomputation.ManageToComputation.CheckProgress:input_type -> managetocomputation.CheckProgressRequest
	6, // 5: managetocomputation.ManageToComputation.ExecuteComputation:output_type -> google.protobuf.Empty
	7, // 6: managetocomputation.ManageToComputation.CheckProgress:output_type -> pb_common_types.JobProgress
	5, // [5:7] is the sub-list for method output_type
	3, // [3:5] is the sub-list for method input_type
	3, // [3:3] is the sub-list for extension type_name
	3, // [3:3] is the sub-list for extension extendee
	0, // [0:3] is the sub-list for field type_name
}

func init() { file_manage_to_computation_proto_init() }
func file_manage_to_computation_proto_init() {
	if File_manage_to_computation_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_manage_to_computation_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*JoinOrder); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_manage_to_computation_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Input); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_manage_to_computation_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*ExecuteComputationRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_manage_to_computation_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CheckStateResponse); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_manage_to_computation_proto_msgTypes[4].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CheckProgressRequest); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_manage_to_computation_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   5,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_manage_to_computation_proto_goTypes,
		DependencyIndexes: file_manage_to_computation_proto_depIdxs,
		MessageInfos:      file_manage_to_computation_proto_msgTypes,
	}.Build()
	File_manage_to_computation_proto = out.File
	file_manage_to_computation_proto_rawDesc = nil
	file_manage_to_computation_proto_goTypes = nil
	file_manage_to_computation_proto_depIdxs = nil
}
