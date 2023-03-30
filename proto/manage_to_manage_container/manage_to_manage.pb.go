// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.28.0
// 	protoc        v3.19.1
// source: manage_to_manage.proto

package manage_to_manage_container

import (
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
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
// the message of DeleteSharesRequest
type DeleteSharesRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	DataId string `protobuf:"bytes,1,opt,name=data_id,json=dataId,proto3" json:"data_id,omitempty"`
}

func (x *DeleteSharesRequest) Reset() {
	*x = DeleteSharesRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *DeleteSharesRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*DeleteSharesRequest) ProtoMessage() {}

func (x *DeleteSharesRequest) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use DeleteSharesRequest.ProtoReflect.Descriptor instead.
func (*DeleteSharesRequest) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{0}
}

func (x *DeleteSharesRequest) GetDataId() string {
	if x != nil {
		return x.DataId
	}
	return ""
}

//*
// the message of DeleteSharesResponse
type DeleteSharesResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Message string `protobuf:"bytes,1,opt,name=message,proto3" json:"message,omitempty"`
	IsOk    bool   `protobuf:"varint,2,opt,name=is_ok,json=isOk,proto3" json:"is_ok,omitempty"`
}

func (x *DeleteSharesResponse) Reset() {
	*x = DeleteSharesResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *DeleteSharesResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*DeleteSharesResponse) ProtoMessage() {}

func (x *DeleteSharesResponse) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use DeleteSharesResponse.ProtoReflect.Descriptor instead.
func (*DeleteSharesResponse) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{1}
}

func (x *DeleteSharesResponse) GetMessage() string {
	if x != nil {
		return x.Message
	}
	return ""
}

func (x *DeleteSharesResponse) GetIsOk() bool {
	if x != nil {
		return x.IsOk
	}
	return false
}

//*
// the message of SyncRequest
type SyncRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	SyncId string `protobuf:"bytes,1,opt,name=sync_id,json=syncId,proto3" json:"sync_id,omitempty"`
}

func (x *SyncRequest) Reset() {
	*x = SyncRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *SyncRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*SyncRequest) ProtoMessage() {}

func (x *SyncRequest) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use SyncRequest.ProtoReflect.Descriptor instead.
func (*SyncRequest) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{2}
}

func (x *SyncRequest) GetSyncId() string {
	if x != nil {
		return x.SyncId
	}
	return ""
}

//*
// the message of SyncResponse
type SyncResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Message string `protobuf:"bytes,1,opt,name=message,proto3" json:"message,omitempty"`
	IsOk    bool   `protobuf:"varint,2,opt,name=is_ok,json=isOk,proto3" json:"is_ok,omitempty"`
}

func (x *SyncResponse) Reset() {
	*x = SyncResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *SyncResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*SyncResponse) ProtoMessage() {}

func (x *SyncResponse) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use SyncResponse.ProtoReflect.Descriptor instead.
func (*SyncResponse) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{3}
}

func (x *SyncResponse) GetMessage() string {
	if x != nil {
		return x.Message
	}
	return ""
}

func (x *SyncResponse) GetIsOk() bool {
	if x != nil {
		return x.IsOk
	}
	return false
}

type CreateStatusFileRequest struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	JobUuid string `protobuf:"bytes,1,opt,name=job_uuid,json=jobUuid,proto3" json:"job_uuid,omitempty"`
}

func (x *CreateStatusFileRequest) Reset() {
	*x = CreateStatusFileRequest{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[4]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateStatusFileRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateStatusFileRequest) ProtoMessage() {}

func (x *CreateStatusFileRequest) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[4]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateStatusFileRequest.ProtoReflect.Descriptor instead.
func (*CreateStatusFileRequest) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{4}
}

func (x *CreateStatusFileRequest) GetJobUuid() string {
	if x != nil {
		return x.JobUuid
	}
	return ""
}

type CreateStatusFileResponse struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Message string `protobuf:"bytes,1,opt,name=message,proto3" json:"message,omitempty"`
	IsOk    bool   `protobuf:"varint,2,opt,name=is_ok,json=isOk,proto3" json:"is_ok,omitempty"`
}

func (x *CreateStatusFileResponse) Reset() {
	*x = CreateStatusFileResponse{}
	if protoimpl.UnsafeEnabled {
		mi := &file_manage_to_manage_proto_msgTypes[5]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *CreateStatusFileResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CreateStatusFileResponse) ProtoMessage() {}

func (x *CreateStatusFileResponse) ProtoReflect() protoreflect.Message {
	mi := &file_manage_to_manage_proto_msgTypes[5]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CreateStatusFileResponse.ProtoReflect.Descriptor instead.
func (*CreateStatusFileResponse) Descriptor() ([]byte, []int) {
	return file_manage_to_manage_proto_rawDescGZIP(), []int{5}
}

func (x *CreateStatusFileResponse) GetMessage() string {
	if x != nil {
		return x.Message
	}
	return ""
}

func (x *CreateStatusFileResponse) GetIsOk() bool {
	if x != nil {
		return x.IsOk
	}
	return false
}

var File_manage_to_manage_proto protoreflect.FileDescriptor

var file_manage_to_manage_proto_rawDesc = []byte{
	0x0a, 0x16, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x5f, 0x74, 0x6f, 0x5f, 0x6d, 0x61, 0x6e, 0x61,
	0x67, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x0e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65,
	0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x22, 0x2e, 0x0a, 0x13, 0x44, 0x65, 0x6c, 0x65,
	0x74, 0x65, 0x53, 0x68, 0x61, 0x72, 0x65, 0x73, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12,
	0x17, 0x0a, 0x07, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09,
	0x52, 0x06, 0x64, 0x61, 0x74, 0x61, 0x49, 0x64, 0x22, 0x45, 0x0a, 0x14, 0x44, 0x65, 0x6c, 0x65,
	0x74, 0x65, 0x53, 0x68, 0x61, 0x72, 0x65, 0x73, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65,
	0x12, 0x18, 0x0a, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28,
	0x09, 0x52, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x12, 0x13, 0x0a, 0x05, 0x69, 0x73,
	0x5f, 0x6f, 0x6b, 0x18, 0x02, 0x20, 0x01, 0x28, 0x08, 0x52, 0x04, 0x69, 0x73, 0x4f, 0x6b, 0x22,
	0x26, 0x0a, 0x0b, 0x53, 0x79, 0x6e, 0x63, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x12, 0x17,
	0x0a, 0x07, 0x73, 0x79, 0x6e, 0x63, 0x5f, 0x69, 0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52,
	0x06, 0x73, 0x79, 0x6e, 0x63, 0x49, 0x64, 0x22, 0x3d, 0x0a, 0x0c, 0x53, 0x79, 0x6e, 0x63, 0x52,
	0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x18, 0x0a, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61,
	0x67, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67,
	0x65, 0x12, 0x13, 0x0a, 0x05, 0x69, 0x73, 0x5f, 0x6f, 0x6b, 0x18, 0x02, 0x20, 0x01, 0x28, 0x08,
	0x52, 0x04, 0x69, 0x73, 0x4f, 0x6b, 0x22, 0x34, 0x0a, 0x17, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65,
	0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x46, 0x69, 0x6c, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73,
	0x74, 0x12, 0x19, 0x0a, 0x08, 0x6a, 0x6f, 0x62, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x01, 0x20,
	0x01, 0x28, 0x09, 0x52, 0x07, 0x6a, 0x6f, 0x62, 0x55, 0x75, 0x69, 0x64, 0x22, 0x49, 0x0a, 0x18,
	0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x46, 0x69, 0x6c, 0x65,
	0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0x18, 0x0a, 0x07, 0x6d, 0x65, 0x73, 0x73,
	0x61, 0x67, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x6d, 0x65, 0x73, 0x73, 0x61,
	0x67, 0x65, 0x12, 0x13, 0x0a, 0x05, 0x69, 0x73, 0x5f, 0x6f, 0x6b, 0x18, 0x02, 0x20, 0x01, 0x28,
	0x08, 0x52, 0x04, 0x69, 0x73, 0x4f, 0x6b, 0x32, 0x9b, 0x02, 0x0a, 0x0e, 0x4d, 0x61, 0x6e, 0x61,
	0x67, 0x65, 0x54, 0x6f, 0x4d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x12, 0x5b, 0x0a, 0x0c, 0x44, 0x65,
	0x6c, 0x65, 0x74, 0x65, 0x53, 0x68, 0x61, 0x72, 0x65, 0x73, 0x12, 0x23, 0x2e, 0x6d, 0x61, 0x6e,
	0x61, 0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x2e, 0x44, 0x65, 0x6c, 0x65,
	0x74, 0x65, 0x53, 0x68, 0x61, 0x72, 0x65, 0x73, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a,
	0x24, 0x2e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65,
	0x2e, 0x44, 0x65, 0x6c, 0x65, 0x74, 0x65, 0x53, 0x68, 0x61, 0x72, 0x65, 0x73, 0x52, 0x65, 0x73,
	0x70, 0x6f, 0x6e, 0x73, 0x65, 0x22, 0x00, 0x12, 0x43, 0x0a, 0x04, 0x53, 0x79, 0x6e, 0x63, 0x12,
	0x1b, 0x2e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65,
	0x2e, 0x53, 0x79, 0x6e, 0x63, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x1c, 0x2e, 0x6d,
	0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x2e, 0x53, 0x79,
	0x6e, 0x63, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x22, 0x00, 0x12, 0x67, 0x0a, 0x10,
	0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x46, 0x69, 0x6c, 0x65,
	0x12, 0x27, 0x2e, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67,
	0x65, 0x2e, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x46, 0x69,
	0x6c, 0x65, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x1a, 0x28, 0x2e, 0x6d, 0x61, 0x6e, 0x61,
	0x67, 0x65, 0x74, 0x6f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x2e, 0x43, 0x72, 0x65, 0x61, 0x74,
	0x65, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x46, 0x69, 0x6c, 0x65, 0x52, 0x65, 0x73, 0x70, 0x6f,
	0x6e, 0x73, 0x65, 0x22, 0x00, 0x42, 0x47, 0x5a, 0x45, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2e,
	0x63, 0x6f, 0x6d, 0x2f, 0x61, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x2d, 0x64, 0x65, 0x76,
	0x65, 0x6c, 0x6f, 0x70, 0x2f, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x4d, 0x50, 0x43, 0x2f, 0x70, 0x72,
	0x6f, 0x74, 0x6f, 0x2f, 0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x5f, 0x74, 0x6f, 0x5f, 0x6d, 0x61,
	0x6e, 0x61, 0x67, 0x65, 0x5f, 0x63, 0x6f, 0x6e, 0x74, 0x61, 0x69, 0x6e, 0x65, 0x72, 0x62, 0x06,
	0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_manage_to_manage_proto_rawDescOnce sync.Once
	file_manage_to_manage_proto_rawDescData = file_manage_to_manage_proto_rawDesc
)

func file_manage_to_manage_proto_rawDescGZIP() []byte {
	file_manage_to_manage_proto_rawDescOnce.Do(func() {
		file_manage_to_manage_proto_rawDescData = protoimpl.X.CompressGZIP(file_manage_to_manage_proto_rawDescData)
	})
	return file_manage_to_manage_proto_rawDescData
}

var file_manage_to_manage_proto_msgTypes = make([]protoimpl.MessageInfo, 6)
var file_manage_to_manage_proto_goTypes = []interface{}{
	(*DeleteSharesRequest)(nil),      // 0: managetomanage.DeleteSharesRequest
	(*DeleteSharesResponse)(nil),     // 1: managetomanage.DeleteSharesResponse
	(*SyncRequest)(nil),              // 2: managetomanage.SyncRequest
	(*SyncResponse)(nil),             // 3: managetomanage.SyncResponse
	(*CreateStatusFileRequest)(nil),  // 4: managetomanage.CreateStatusFileRequest
	(*CreateStatusFileResponse)(nil), // 5: managetomanage.CreateStatusFileResponse
}
var file_manage_to_manage_proto_depIdxs = []int32{
	0, // 0: managetomanage.ManageToManage.DeleteShares:input_type -> managetomanage.DeleteSharesRequest
	2, // 1: managetomanage.ManageToManage.Sync:input_type -> managetomanage.SyncRequest
	4, // 2: managetomanage.ManageToManage.CreateStatusFile:input_type -> managetomanage.CreateStatusFileRequest
	1, // 3: managetomanage.ManageToManage.DeleteShares:output_type -> managetomanage.DeleteSharesResponse
	3, // 4: managetomanage.ManageToManage.Sync:output_type -> managetomanage.SyncResponse
	5, // 5: managetomanage.ManageToManage.CreateStatusFile:output_type -> managetomanage.CreateStatusFileResponse
	3, // [3:6] is the sub-list for method output_type
	0, // [0:3] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_manage_to_manage_proto_init() }
func file_manage_to_manage_proto_init() {
	if File_manage_to_manage_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_manage_to_manage_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*DeleteSharesRequest); i {
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
		file_manage_to_manage_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*DeleteSharesResponse); i {
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
		file_manage_to_manage_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*SyncRequest); i {
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
		file_manage_to_manage_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*SyncResponse); i {
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
		file_manage_to_manage_proto_msgTypes[4].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateStatusFileRequest); i {
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
		file_manage_to_manage_proto_msgTypes[5].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*CreateStatusFileResponse); i {
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
			RawDescriptor: file_manage_to_manage_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   6,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_manage_to_manage_proto_goTypes,
		DependencyIndexes: file_manage_to_manage_proto_depIdxs,
		MessageInfos:      file_manage_to_manage_proto_msgTypes,
	}.Build()
	File_manage_to_manage_proto = out.File
	file_manage_to_manage_proto_rawDesc = nil
	file_manage_to_manage_proto_goTypes = nil
	file_manage_to_manage_proto_depIdxs = nil
}
