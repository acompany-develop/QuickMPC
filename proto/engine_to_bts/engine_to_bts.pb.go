// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.28.0
// 	protoc        v3.19.1
// source: engine_to_bts.proto

package engine_to_bts

import (
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

type Type int32

const (
	Type_TYPE_UNKNOWN    Type = 0
	Type_TYPE_FLOAT      Type = 1
	Type_TYPE_FIXEDPOINT Type = 2
)

// Enum value maps for Type.
var (
	Type_name = map[int32]string{
		0: "TYPE_UNKNOWN",
		1: "TYPE_FLOAT",
		2: "TYPE_FIXEDPOINT",
	}
	Type_value = map[string]int32{
		"TYPE_UNKNOWN":    0,
		"TYPE_FLOAT":      1,
		"TYPE_FIXEDPOINT": 2,
	}
)

func (x Type) Enum() *Type {
	p := new(Type)
	*p = x
	return p
}

func (x Type) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (Type) Descriptor() protoreflect.EnumDescriptor {
	return file_engine_to_bts_proto_enumTypes[0].Descriptor()
}

func (Type) Type() protoreflect.EnumType {
	return &file_engine_to_bts_proto_enumTypes[0]
}

func (x Type) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use Type.Descriptor instead.
func (Type) EnumDescriptor() ([]byte, []int) {
	return file_engine_to_bts_proto_rawDescGZIP(), []int{0}
}

type GetTriplesRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	JobId      uint32 `protobuf:"varint,1,opt,name=job_id,json=jobId,proto3" json:"job_id,omitempty"`
	Amount     uint32 `protobuf:"varint,2,opt,name=amount,proto3" json:"amount,omitempty"`
	TripleType Type   `protobuf:"varint,3,opt,name=triple_type,json=tripleType,proto3,enum=enginetobts.Type" json:"triple_type,omitempty"`
	RequestId  uint32 `protobuf:"varint,4,opt,name=request_id,json=requestId,proto3" json:"request_id,omitempty"`
}

func (x *GetTriplesRequest) Reset() {
	*x = GetTriplesRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_engine_to_bts_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetTriplesRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetTriplesRequest) ProtoMessage() {}

func (x *GetTriplesRequest) ProtoReflect() protoreflect.Message {
	mi := &file_engine_to_bts_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetTriplesRequest.ProtoReflect.Descriptor instead.
func (*GetTriplesRequest) Descriptor() ([]byte, []int) {
	return file_engine_to_bts_proto_rawDescGZIP(), []int{0}
}

func (x *GetTriplesRequest) GetJobId() uint32 {
	if x != nil {
		return x.JobId
	}
	return 0
}

func (x *GetTriplesRequest) GetAmount() uint32 {
	if x != nil {
		return x.Amount
	}
	return 0
}

func (x *GetTriplesRequest) GetTripleType() Type {
	if x != nil {
		return x.TripleType
	}
	return Type_TYPE_UNKNOWN
}

func (x *GetTriplesRequest) GetRequestId() uint32 {
	if x != nil {
		return x.RequestId
	}
	return 0
}

type Triple struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	A int64 `protobuf:"varint,2,opt,name=a,proto3" json:"a,omitempty"`
	B int64 `protobuf:"varint,3,opt,name=b,proto3" json:"b,omitempty"`
	C int64 `protobuf:"varint,4,opt,name=c,proto3" json:"c,omitempty"`
}

func (x *Triple) Reset() {
	*x = Triple{}
	if protoimpl.UnsafeEnabled {
		mi := &file_engine_to_bts_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Triple) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Triple) ProtoMessage() {}

func (x *Triple) ProtoReflect() protoreflect.Message {
	mi := &file_engine_to_bts_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Triple.ProtoReflect.Descriptor instead.
func (*Triple) Descriptor() ([]byte, []int) {
	return file_engine_to_bts_proto_rawDescGZIP(), []int{1}
}

func (x *Triple) GetA() int64 {
	if x != nil {
		return x.A
	}
	return 0
}

func (x *Triple) GetB() int64 {
	if x != nil {
		return x.B
	}
	return 0
}

func (x *Triple) GetC() int64 {
	if x != nil {
		return x.C
	}
	return 0
}

type GetTriplesResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Triples []*Triple `protobuf:"bytes,1,rep,name=triples,proto3" json:"triples,omitempty"`
}

func (x *GetTriplesResponse) Reset() {
	*x = GetTriplesResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_engine_to_bts_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *GetTriplesResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GetTriplesResponse) ProtoMessage() {}

func (x *GetTriplesResponse) ProtoReflect() protoreflect.Message {
	mi := &file_engine_to_bts_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GetTriplesResponse.ProtoReflect.Descriptor instead.
func (*GetTriplesResponse) Descriptor() ([]byte, []int) {
	return file_engine_to_bts_proto_rawDescGZIP(), []int{2}
}

func (x *GetTriplesResponse) GetTriples() []*Triple {
	if x != nil {
		return x.Triples
	}
	return nil
}

type DeleteJobIdTripleRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	JobId uint32 `protobuf:"varint,1,opt,name=job_id,json=jobId,proto3" json:"job_id,omitempty"`
}

func (x *DeleteJobIdTripleRequest) Reset() {
	*x = DeleteJobIdTripleRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_engine_to_bts_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *DeleteJobIdTripleRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*DeleteJobIdTripleRequest) ProtoMessage() {}

func (x *DeleteJobIdTripleRequest) ProtoReflect() protoreflect.Message {
	mi := &file_engine_to_bts_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use DeleteJobIdTripleRequest.ProtoReflect.Descriptor instead.
func (*DeleteJobIdTripleRequest) Descriptor() ([]byte, []int) {
	return file_engine_to_bts_proto_rawDescGZIP(), []int{3}
}

func (x *DeleteJobIdTripleRequest) GetJobId() uint32 {
	if x != nil {
		return x.JobId
	}
	return 0
}

var File_engine_to_bts_proto protoreflect.FileDescriptor

var file_engine_to_bts_proto_rawDesc = []byte{
	0x0a, 0x13, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x5f, 0x74, 0x6f, 0x5f, 0x62, 0x74, 0x73, 0x2e,
	0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x0b, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x74, 0x6f, 0x62,
	0x74, 0x73, 0x1a, 0x1b, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2f, 0x70, 0x72, 0x6f, 0x74, 0x6f,
	0x62, 0x75, 0x66, 0x2f, 0x65, 0x6d, 0x70, 0x74, 0x79, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22,
	0x95, 0x01, 0x0a, 0x11, 0x47, 0x65, 0x74, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73, 0x52, 0x65,
	0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x15, 0x0a, 0x06, 0x6a, 0x6f, 0x62, 0x5f, 0x69, 0x64, 0x18,
	0x01, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x05, 0x6a, 0x6f, 0x62, 0x49, 0x64, 0x12, 0x16, 0x0a, 0x06,
	0x61, 0x6d, 0x6f, 0x75, 0x6e, 0x74, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x06, 0x61, 0x6d,
	0x6f, 0x75, 0x6e, 0x74, 0x12, 0x32, 0x0a, 0x0b, 0x74, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x5f, 0x74,
	0x79, 0x70, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0e, 0x32, 0x11, 0x2e, 0x65, 0x6e, 0x67, 0x69,
	0x6e, 0x65, 0x74, 0x6f, 0x62, 0x74, 0x73, 0x2e, 0x54, 0x79, 0x70, 0x65, 0x52, 0x0a, 0x74, 0x72,
	0x69, 0x70, 0x6c, 0x65, 0x54, 0x79, 0x70, 0x65, 0x12, 0x1d, 0x0a, 0x0a, 0x72, 0x65, 0x71, 0x75,
	0x65, 0x73, 0x74, 0x5f, 0x69, 0x64, 0x18, 0x04, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x09, 0x72, 0x65,
	0x71, 0x75, 0x65, 0x73, 0x74, 0x49, 0x64, 0x22, 0x32, 0x0a, 0x06, 0x54, 0x72, 0x69, 0x70, 0x6c,
	0x65, 0x12, 0x0c, 0x0a, 0x01, 0x61, 0x18, 0x02, 0x20, 0x01, 0x28, 0x03, 0x52, 0x01, 0x61, 0x12,
	0x0c, 0x0a, 0x01, 0x62, 0x18, 0x03, 0x20, 0x01, 0x28, 0x03, 0x52, 0x01, 0x62, 0x12, 0x0c, 0x0a,
	0x01, 0x63, 0x18, 0x04, 0x20, 0x01, 0x28, 0x03, 0x52, 0x01, 0x63, 0x22, 0x43, 0x0a, 0x12, 0x47,
	0x65, 0x74, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73,
	0x65, 0x12, 0x2d, 0x0a, 0x07, 0x74, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73, 0x18, 0x01, 0x20, 0x03,
	0x28, 0x0b, 0x32, 0x13, 0x2e, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x74, 0x6f, 0x62, 0x74, 0x73,
	0x2e, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x52, 0x07, 0x74, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73,
	0x22, 0x31, 0x0a, 0x18, 0x44, 0x65, 0x6c, 0x65, 0x74, 0x65, 0x4a, 0x6f, 0x62, 0x49, 0x64, 0x54,
	0x72, 0x69, 0x70, 0x6c, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x15, 0x0a, 0x06,
	0x6a, 0x6f, 0x62, 0x5f, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x05, 0x6a, 0x6f,
	0x62, 0x49, 0x64, 0x2a, 0x3d, 0x0a, 0x04, 0x54, 0x79, 0x70, 0x65, 0x12, 0x10, 0x0a, 0x0c, 0x54,
	0x59, 0x50, 0x45, 0x5f, 0x55, 0x4e, 0x4b, 0x4e, 0x4f, 0x57, 0x4e, 0x10, 0x00, 0x12, 0x0e, 0x0a,
	0x0a, 0x54, 0x59, 0x50, 0x45, 0x5f, 0x46, 0x4c, 0x4f, 0x41, 0x54, 0x10, 0x01, 0x12, 0x13, 0x0a,
	0x0f, 0x54, 0x59, 0x50, 0x45, 0x5f, 0x46, 0x49, 0x58, 0x45, 0x44, 0x50, 0x4f, 0x49, 0x4e, 0x54,
	0x10, 0x02, 0x32, 0xb4, 0x01, 0x0a, 0x0b, 0x45, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x54, 0x6f, 0x42,
	0x74, 0x73, 0x12, 0x4f, 0x0a, 0x0a, 0x47, 0x65, 0x74, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73,
	0x12, 0x1e, 0x2e, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x74, 0x6f, 0x62, 0x74, 0x73, 0x2e, 0x47,
	0x65, 0x74, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74,
	0x1a, 0x1f, 0x2e, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x74, 0x6f, 0x62, 0x74, 0x73, 0x2e, 0x47,
	0x65, 0x74, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x73, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73,
	0x65, 0x22, 0x00, 0x12, 0x54, 0x0a, 0x11, 0x44, 0x65, 0x6c, 0x65, 0x74, 0x65, 0x4a, 0x6f, 0x62,
	0x49, 0x64, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x12, 0x25, 0x2e, 0x65, 0x6e, 0x67, 0x69, 0x6e,
	0x65, 0x74, 0x6f, 0x62, 0x74, 0x73, 0x2e, 0x44, 0x65, 0x6c, 0x65, 0x74, 0x65, 0x4a, 0x6f, 0x62,
	0x49, 0x64, 0x54, 0x72, 0x69, 0x70, 0x6c, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a,
	0x16, 0x2e, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x62, 0x75,
	0x66, 0x2e, 0x45, 0x6d, 0x70, 0x74, 0x79, 0x22, 0x00, 0x42, 0x3a, 0x5a, 0x38, 0x67, 0x69, 0x74,
	0x68, 0x75, 0x62, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x61, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79,
	0x2d, 0x64, 0x65, 0x76, 0x65, 0x6c, 0x6f, 0x70, 0x2f, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x4d, 0x50,
	0x43, 0x2f, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x2f, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x5f, 0x74,
	0x6f, 0x5f, 0x62, 0x74, 0x73, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_engine_to_bts_proto_rawDescOnce sync.Once
	file_engine_to_bts_proto_rawDescData = file_engine_to_bts_proto_rawDesc
)

func file_engine_to_bts_proto_rawDescGZIP() []byte {
	file_engine_to_bts_proto_rawDescOnce.Do(func() {
		file_engine_to_bts_proto_rawDescData = protoimpl.X.CompressGZIP(file_engine_to_bts_proto_rawDescData)
	})
	return file_engine_to_bts_proto_rawDescData
}

var file_engine_to_bts_proto_enumTypes = make([]protoimpl.EnumInfo, 1)
var file_engine_to_bts_proto_msgTypes = make([]protoimpl.MessageInfo, 4)
var file_engine_to_bts_proto_goTypes = []interface{}{
	(Type)(0),                        // 0: enginetobts.Type
	(*GetTriplesRequest)(nil),        // 1: enginetobts.GetTriplesRequest
	(*Triple)(nil),                   // 2: enginetobts.Triple
	(*GetTriplesResponse)(nil),       // 3: enginetobts.GetTriplesResponse
	(*DeleteJobIdTripleRequest)(nil), // 4: enginetobts.DeleteJobIdTripleRequest
	(*emptypb.Empty)(nil),            // 5: google.protobuf.Empty
}
var file_engine_to_bts_proto_depIdxs = []int32{
	0, // 0: enginetobts.GetTriplesRequest.triple_type:type_name -> enginetobts.Type
	2, // 1: enginetobts.GetTriplesResponse.triples:type_name -> enginetobts.Triple
	1, // 2: enginetobts.EngineToBts.GetTriples:input_type -> enginetobts.GetTriplesRequest
	4, // 3: enginetobts.EngineToBts.DeleteJobIdTriple:input_type -> enginetobts.DeleteJobIdTripleRequest
	3, // 4: enginetobts.EngineToBts.GetTriples:output_type -> enginetobts.GetTriplesResponse
	5, // 5: enginetobts.EngineToBts.DeleteJobIdTriple:output_type -> google.protobuf.Empty
	4, // [4:6] is the sub-list for method output_type
	2, // [2:4] is the sub-list for method input_type
	2, // [2:2] is the sub-list for extension type_name
	2, // [2:2] is the sub-list for extension extendee
	0, // [0:2] is the sub-list for field type_name
}

func init() { file_engine_to_bts_proto_init() }
func file_engine_to_bts_proto_init() {
	if File_engine_to_bts_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_engine_to_bts_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetTriplesRequest); i {
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
		file_engine_to_bts_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Triple); i {
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
		file_engine_to_bts_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*GetTriplesResponse); i {
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
		file_engine_to_bts_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*DeleteJobIdTripleRequest); i {
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
			RawDescriptor: file_engine_to_bts_proto_rawDesc,
			NumEnums:      1,
			NumMessages:   4,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_engine_to_bts_proto_goTypes,
		DependencyIndexes: file_engine_to_bts_proto_depIdxs,
		EnumInfos:         file_engine_to_bts_proto_enumTypes,
		MessageInfos:      file_engine_to_bts_proto_msgTypes,
	}.Build()
	File_engine_to_bts_proto = out.File
	file_engine_to_bts_proto_rawDesc = nil
	file_engine_to_bts_proto_goTypes = nil
	file_engine_to_bts_proto_depIdxs = nil
}
