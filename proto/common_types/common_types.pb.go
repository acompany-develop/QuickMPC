// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.28.0
// 	protoc        v3.19.1
// source: common_types.proto

package common_types

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

type JobStatus int32

const (
	JobStatus_UNKNOWN   JobStatus = 0
	JobStatus_ERROR     JobStatus = 1
	JobStatus_RECEIVED  JobStatus = 2
	JobStatus_PRE_JOB   JobStatus = 3
	JobStatus_READ_DB   JobStatus = 4
	JobStatus_COMPUTE   JobStatus = 5
	JobStatus_COMPLETED JobStatus = 6
)

// Enum value maps for JobStatus.
var (
	JobStatus_name = map[int32]string{
		0: "UNKNOWN",
		1: "ERROR",
		2: "RECEIVED",
		3: "PRE_JOB",
		4: "READ_DB",
		5: "COMPUTE",
		6: "COMPLETED",
	}
	JobStatus_value = map[string]int32{
		"UNKNOWN":   0,
		"ERROR":     1,
		"RECEIVED":  2,
		"PRE_JOB":   3,
		"READ_DB":   4,
		"COMPUTE":   5,
		"COMPLETED": 6,
	}
)

func (x JobStatus) Enum() *JobStatus {
	p := new(JobStatus)
	*p = x
	return p
}

func (x JobStatus) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (JobStatus) Descriptor() protoreflect.EnumDescriptor {
	return file_common_types_proto_enumTypes[0].Descriptor()
}

func (JobStatus) Type() protoreflect.EnumType {
	return &file_common_types_proto_enumTypes[0]
}

func (x JobStatus) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use JobStatus.Descriptor instead.
func (JobStatus) EnumDescriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{0}
}

type ComputationMethod int32

const (
	ComputationMethod_COMPUTATION_METHOD_UNSPECIFIED ComputationMethod = 0
	ComputationMethod_COMPUTATION_METHOD_MEAN        ComputationMethod = 1
	ComputationMethod_COMPUTATION_METHOD_VARIANCE    ComputationMethod = 2
	ComputationMethod_COMPUTATION_METHOD_SUM         ComputationMethod = 3
	ComputationMethod_COMPUTATION_METHOD_CORREL      ComputationMethod = 4
	ComputationMethod_COMPUTATION_METHOD_MESH_CODE   ComputationMethod = 5
	ComputationMethod_COMPUTATION_METHOD_JOIN_TABLE  ComputationMethod = 6
)

// Enum value maps for ComputationMethod.
var (
	ComputationMethod_name = map[int32]string{
		0: "COMPUTATION_METHOD_UNSPECIFIED",
		1: "COMPUTATION_METHOD_MEAN",
		2: "COMPUTATION_METHOD_VARIANCE",
		3: "COMPUTATION_METHOD_SUM",
		4: "COMPUTATION_METHOD_CORREL",
		5: "COMPUTATION_METHOD_MESH_CODE",
		6: "COMPUTATION_METHOD_JOIN_TABLE",
	}
	ComputationMethod_value = map[string]int32{
		"COMPUTATION_METHOD_UNSPECIFIED": 0,
		"COMPUTATION_METHOD_MEAN":        1,
		"COMPUTATION_METHOD_VARIANCE":    2,
		"COMPUTATION_METHOD_SUM":         3,
		"COMPUTATION_METHOD_CORREL":      4,
		"COMPUTATION_METHOD_MESH_CODE":   5,
		"COMPUTATION_METHOD_JOIN_TABLE":  6,
	}
)

func (x ComputationMethod) Enum() *ComputationMethod {
	p := new(ComputationMethod)
	*p = x
	return p
}

func (x ComputationMethod) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (ComputationMethod) Descriptor() protoreflect.EnumDescriptor {
	return file_common_types_proto_enumTypes[1].Descriptor()
}

func (ComputationMethod) Type() protoreflect.EnumType {
	return &file_common_types_proto_enumTypes[1]
}

func (x ComputationMethod) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use ComputationMethod.Descriptor instead.
func (ComputationMethod) EnumDescriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{1}
}

type ShareValueTypeEnum int32

const (
	ShareValueTypeEnum_SHARE_VALUE_TYPE_UNSPECIFIED                  ShareValueTypeEnum = 0
	ShareValueTypeEnum_SHARE_VALUE_TYPE_FIXED_POINT                  ShareValueTypeEnum = 1
	ShareValueTypeEnum_SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION ShareValueTypeEnum = 2
)

// Enum value maps for ShareValueTypeEnum.
var (
	ShareValueTypeEnum_name = map[int32]string{
		0: "SHARE_VALUE_TYPE_UNSPECIFIED",
		1: "SHARE_VALUE_TYPE_FIXED_POINT",
		2: "SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION",
	}
	ShareValueTypeEnum_value = map[string]int32{
		"SHARE_VALUE_TYPE_UNSPECIFIED":                  0,
		"SHARE_VALUE_TYPE_FIXED_POINT":                  1,
		"SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION": 2,
	}
)

func (x ShareValueTypeEnum) Enum() *ShareValueTypeEnum {
	p := new(ShareValueTypeEnum)
	*p = x
	return p
}

func (x ShareValueTypeEnum) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (ShareValueTypeEnum) Descriptor() protoreflect.EnumDescriptor {
	return file_common_types_proto_enumTypes[2].Descriptor()
}

func (ShareValueTypeEnum) Type() protoreflect.EnumType {
	return &file_common_types_proto_enumTypes[2]
}

func (x ShareValueTypeEnum) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use ShareValueTypeEnum.Descriptor instead.
func (ShareValueTypeEnum) EnumDescriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{2}
}

type ProcedureProgress struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Id          uint64  `protobuf:"varint,1,opt,name=id,proto3" json:"id,omitempty"`                  // unique id in ProgressManager context
	Description string  `protobuf:"bytes,2,opt,name=description,proto3" json:"description,omitempty"` // describe specific process in job
	Progress    float32 `protobuf:"fixed32,3,opt,name=progress,proto3" json:"progress,omitempty"`     // describe progress by [0.0, 100.0]
	Completed   bool    `protobuf:"varint,4,opt,name=completed,proto3" json:"completed,omitempty"`    // describe that procedure is completed or not
	Details     *string `protobuf:"bytes,5,opt,name=details,proto3,oneof" json:"details,omitempty"`   // describe progress in details
}

func (x *ProcedureProgress) Reset() {
	*x = ProcedureProgress{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *ProcedureProgress) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*ProcedureProgress) ProtoMessage() {}

func (x *ProcedureProgress) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use ProcedureProgress.ProtoReflect.Descriptor instead.
func (*ProcedureProgress) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{0}
}

func (x *ProcedureProgress) GetId() uint64 {
	if x != nil {
		return x.Id
	}
	return 0
}

func (x *ProcedureProgress) GetDescription() string {
	if x != nil {
		return x.Description
	}
	return ""
}

func (x *ProcedureProgress) GetProgress() float32 {
	if x != nil {
		return x.Progress
	}
	return 0
}

func (x *ProcedureProgress) GetCompleted() bool {
	if x != nil {
		return x.Completed
	}
	return false
}

func (x *ProcedureProgress) GetDetails() string {
	if x != nil && x.Details != nil {
		return *x.Details
	}
	return ""
}

type JobProgress struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	JobUuid    string               `protobuf:"bytes,1,opt,name=job_uuid,json=jobUuid,proto3" json:"job_uuid,omitempty"`
	Status     JobStatus            `protobuf:"varint,2,opt,name=status,proto3,enum=pb_common_types.JobStatus" json:"status,omitempty"`
	Progresses []*ProcedureProgress `protobuf:"bytes,3,rep,name=progresses,proto3" json:"progresses,omitempty"`
}

func (x *JobProgress) Reset() {
	*x = JobProgress{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *JobProgress) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JobProgress) ProtoMessage() {}

func (x *JobProgress) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JobProgress.ProtoReflect.Descriptor instead.
func (*JobProgress) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{1}
}

func (x *JobProgress) GetJobUuid() string {
	if x != nil {
		return x.JobUuid
	}
	return ""
}

func (x *JobProgress) GetStatus() JobStatus {
	if x != nil {
		return x.Status
	}
	return JobStatus_UNKNOWN
}

func (x *JobProgress) GetProgresses() []*ProcedureProgress {
	if x != nil {
		return x.Progresses
	}
	return nil
}

type Stacktrace struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Frames []*Stacktrace_Frame `protobuf:"bytes,1,rep,name=frames,proto3" json:"frames,omitempty"`
}

func (x *Stacktrace) Reset() {
	*x = Stacktrace{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Stacktrace) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Stacktrace) ProtoMessage() {}

func (x *Stacktrace) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Stacktrace.ProtoReflect.Descriptor instead.
func (*Stacktrace) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{2}
}

func (x *Stacktrace) GetFrames() []*Stacktrace_Frame {
	if x != nil {
		return x.Frames
	}
	return nil
}

type JobErrorInfo struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	What           string      `protobuf:"bytes,1,opt,name=what,proto3" json:"what,omitempty"`
	AdditionalInfo *string     `protobuf:"bytes,2,opt,name=additional_info,json=additionalInfo,proto3,oneof" json:"additional_info,omitempty"`
	Stacktrace     *Stacktrace `protobuf:"bytes,3,opt,name=stacktrace,proto3,oneof" json:"stacktrace,omitempty"`
}

func (x *JobErrorInfo) Reset() {
	*x = JobErrorInfo{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *JobErrorInfo) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JobErrorInfo) ProtoMessage() {}

func (x *JobErrorInfo) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JobErrorInfo.ProtoReflect.Descriptor instead.
func (*JobErrorInfo) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{3}
}

func (x *JobErrorInfo) GetWhat() string {
	if x != nil {
		return x.What
	}
	return ""
}

func (x *JobErrorInfo) GetAdditionalInfo() string {
	if x != nil && x.AdditionalInfo != nil {
		return *x.AdditionalInfo
	}
	return ""
}

func (x *JobErrorInfo) GetStacktrace() *Stacktrace {
	if x != nil {
		return x.Stacktrace
	}
	return nil
}

type Schema struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Name string             `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
	Type ShareValueTypeEnum `protobuf:"varint,2,opt,name=type,proto3,enum=pb_common_types.ShareValueTypeEnum" json:"type,omitempty"`
}

func (x *Schema) Reset() {
	*x = Schema{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[4]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Schema) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Schema) ProtoMessage() {}

func (x *Schema) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[4]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Schema.ProtoReflect.Descriptor instead.
func (*Schema) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{4}
}

func (x *Schema) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

func (x *Schema) GetType() ShareValueTypeEnum {
	if x != nil {
		return x.Type
	}
	return ShareValueTypeEnum_SHARE_VALUE_TYPE_UNSPECIFIED
}

type Stacktrace_Frame struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	SourceLocation string `protobuf:"bytes,1,opt,name=source_location,json=sourceLocation,proto3" json:"source_location,omitempty"`
	SourceLine     uint64 `protobuf:"varint,2,opt,name=source_line,json=sourceLine,proto3" json:"source_line,omitempty"`
	FunctionName   string `protobuf:"bytes,3,opt,name=function_name,json=functionName,proto3" json:"function_name,omitempty"`
}

func (x *Stacktrace_Frame) Reset() {
	*x = Stacktrace_Frame{}
	if protoimpl.UnsafeEnabled {
		mi := &file_common_types_proto_msgTypes[5]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Stacktrace_Frame) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Stacktrace_Frame) ProtoMessage() {}

func (x *Stacktrace_Frame) ProtoReflect() protoreflect.Message {
	mi := &file_common_types_proto_msgTypes[5]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Stacktrace_Frame.ProtoReflect.Descriptor instead.
func (*Stacktrace_Frame) Descriptor() ([]byte, []int) {
	return file_common_types_proto_rawDescGZIP(), []int{2, 0}
}

func (x *Stacktrace_Frame) GetSourceLocation() string {
	if x != nil {
		return x.SourceLocation
	}
	return ""
}

func (x *Stacktrace_Frame) GetSourceLine() uint64 {
	if x != nil {
		return x.SourceLine
	}
	return 0
}

func (x *Stacktrace_Frame) GetFunctionName() string {
	if x != nil {
		return x.FunctionName
	}
	return ""
}

var File_common_types_proto protoreflect.FileDescriptor

var file_common_types_proto_rawDesc = []byte{
	0x0a, 0x12, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x70,
	0x72, 0x6f, 0x74, 0x6f, 0x12, 0x0f, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f,
	0x74, 0x79, 0x70, 0x65, 0x73, 0x22, 0xaa, 0x01, 0x0a, 0x11, 0x50, 0x72, 0x6f, 0x63, 0x65, 0x64,
	0x75, 0x72, 0x65, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x12, 0x0e, 0x0a, 0x02, 0x69,
	0x64, 0x18, 0x01, 0x20, 0x01, 0x28, 0x04, 0x52, 0x02, 0x69, 0x64, 0x12, 0x20, 0x0a, 0x0b, 0x64,
	0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x18, 0x02, 0x20, 0x01, 0x28, 0x09,
	0x52, 0x0b, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x12, 0x1a, 0x0a,
	0x08, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x18, 0x03, 0x20, 0x01, 0x28, 0x02, 0x52,
	0x08, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x12, 0x1c, 0x0a, 0x09, 0x63, 0x6f, 0x6d,
	0x70, 0x6c, 0x65, 0x74, 0x65, 0x64, 0x18, 0x04, 0x20, 0x01, 0x28, 0x08, 0x52, 0x09, 0x63, 0x6f,
	0x6d, 0x70, 0x6c, 0x65, 0x74, 0x65, 0x64, 0x12, 0x1d, 0x0a, 0x07, 0x64, 0x65, 0x74, 0x61, 0x69,
	0x6c, 0x73, 0x18, 0x05, 0x20, 0x01, 0x28, 0x09, 0x48, 0x00, 0x52, 0x07, 0x64, 0x65, 0x74, 0x61,
	0x69, 0x6c, 0x73, 0x88, 0x01, 0x01, 0x42, 0x0a, 0x0a, 0x08, 0x5f, 0x64, 0x65, 0x74, 0x61, 0x69,
	0x6c, 0x73, 0x22, 0xa0, 0x01, 0x0a, 0x0b, 0x4a, 0x6f, 0x62, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65,
	0x73, 0x73, 0x12, 0x19, 0x0a, 0x08, 0x6a, 0x6f, 0x62, 0x5f, 0x75, 0x75, 0x69, 0x64, 0x18, 0x01,
	0x20, 0x01, 0x28, 0x09, 0x52, 0x07, 0x6a, 0x6f, 0x62, 0x55, 0x75, 0x69, 0x64, 0x12, 0x32, 0x0a,
	0x06, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0e, 0x32, 0x1a, 0x2e,
	0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e,
	0x4a, 0x6f, 0x62, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x52, 0x06, 0x73, 0x74, 0x61, 0x74, 0x75,
	0x73, 0x12, 0x42, 0x0a, 0x0a, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x65, 0x73, 0x18,
	0x03, 0x20, 0x03, 0x28, 0x0b, 0x32, 0x22, 0x2e, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f,
	0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x50, 0x72, 0x6f, 0x63, 0x65, 0x64, 0x75, 0x72,
	0x65, 0x50, 0x72, 0x6f, 0x67, 0x72, 0x65, 0x73, 0x73, 0x52, 0x0a, 0x70, 0x72, 0x6f, 0x67, 0x72,
	0x65, 0x73, 0x73, 0x65, 0x73, 0x22, 0xbf, 0x01, 0x0a, 0x0a, 0x53, 0x74, 0x61, 0x63, 0x6b, 0x74,
	0x72, 0x61, 0x63, 0x65, 0x12, 0x39, 0x0a, 0x06, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x73, 0x18, 0x01,
	0x20, 0x03, 0x28, 0x0b, 0x32, 0x21, 0x2e, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e,
	0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x53, 0x74, 0x61, 0x63, 0x6b, 0x74, 0x72, 0x61, 0x63,
	0x65, 0x2e, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x52, 0x06, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x73, 0x1a,
	0x76, 0x0a, 0x05, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x12, 0x27, 0x0a, 0x0f, 0x73, 0x6f, 0x75, 0x72,
	0x63, 0x65, 0x5f, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x18, 0x01, 0x20, 0x01, 0x28,
	0x09, 0x52, 0x0e, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x4c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f,
	0x6e, 0x12, 0x1f, 0x0a, 0x0b, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x5f, 0x6c, 0x69, 0x6e, 0x65,
	0x18, 0x02, 0x20, 0x01, 0x28, 0x04, 0x52, 0x0a, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x4c, 0x69,
	0x6e, 0x65, 0x12, 0x23, 0x0a, 0x0d, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x6e,
	0x61, 0x6d, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x09, 0x52, 0x0c, 0x66, 0x75, 0x6e, 0x63, 0x74,
	0x69, 0x6f, 0x6e, 0x4e, 0x61, 0x6d, 0x65, 0x22, 0xb5, 0x01, 0x0a, 0x0c, 0x4a, 0x6f, 0x62, 0x45,
	0x72, 0x72, 0x6f, 0x72, 0x49, 0x6e, 0x66, 0x6f, 0x12, 0x12, 0x0a, 0x04, 0x77, 0x68, 0x61, 0x74,
	0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x77, 0x68, 0x61, 0x74, 0x12, 0x2c, 0x0a, 0x0f,
	0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x61, 0x6c, 0x5f, 0x69, 0x6e, 0x66, 0x6f, 0x18,
	0x02, 0x20, 0x01, 0x28, 0x09, 0x48, 0x00, 0x52, 0x0e, 0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f,
	0x6e, 0x61, 0x6c, 0x49, 0x6e, 0x66, 0x6f, 0x88, 0x01, 0x01, 0x12, 0x40, 0x0a, 0x0a, 0x73, 0x74,
	0x61, 0x63, 0x6b, 0x74, 0x72, 0x61, 0x63, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1b,
	0x2e, 0x70, 0x62, 0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73,
	0x2e, 0x53, 0x74, 0x61, 0x63, 0x6b, 0x74, 0x72, 0x61, 0x63, 0x65, 0x48, 0x01, 0x52, 0x0a, 0x73,
	0x74, 0x61, 0x63, 0x6b, 0x74, 0x72, 0x61, 0x63, 0x65, 0x88, 0x01, 0x01, 0x42, 0x12, 0x0a, 0x10,
	0x5f, 0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x61, 0x6c, 0x5f, 0x69, 0x6e, 0x66, 0x6f,
	0x42, 0x0d, 0x0a, 0x0b, 0x5f, 0x73, 0x74, 0x61, 0x63, 0x6b, 0x74, 0x72, 0x61, 0x63, 0x65, 0x22,
	0x55, 0x0a, 0x06, 0x53, 0x63, 0x68, 0x65, 0x6d, 0x61, 0x12, 0x12, 0x0a, 0x04, 0x6e, 0x61, 0x6d,
	0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x09, 0x52, 0x04, 0x6e, 0x61, 0x6d, 0x65, 0x12, 0x37, 0x0a,
	0x04, 0x74, 0x79, 0x70, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0e, 0x32, 0x23, 0x2e, 0x70, 0x62,
	0x5f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2e, 0x53, 0x68,
	0x61, 0x72, 0x65, 0x56, 0x61, 0x6c, 0x75, 0x65, 0x54, 0x79, 0x70, 0x65, 0x45, 0x6e, 0x75, 0x6d,
	0x52, 0x04, 0x74, 0x79, 0x70, 0x65, 0x2a, 0x67, 0x0a, 0x09, 0x4a, 0x6f, 0x62, 0x53, 0x74, 0x61,
	0x74, 0x75, 0x73, 0x12, 0x0b, 0x0a, 0x07, 0x55, 0x4e, 0x4b, 0x4e, 0x4f, 0x57, 0x4e, 0x10, 0x00,
	0x12, 0x09, 0x0a, 0x05, 0x45, 0x52, 0x52, 0x4f, 0x52, 0x10, 0x01, 0x12, 0x0c, 0x0a, 0x08, 0x52,
	0x45, 0x43, 0x45, 0x49, 0x56, 0x45, 0x44, 0x10, 0x02, 0x12, 0x0b, 0x0a, 0x07, 0x50, 0x52, 0x45,
	0x5f, 0x4a, 0x4f, 0x42, 0x10, 0x03, 0x12, 0x0b, 0x0a, 0x07, 0x52, 0x45, 0x41, 0x44, 0x5f, 0x44,
	0x42, 0x10, 0x04, 0x12, 0x0b, 0x0a, 0x07, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54, 0x45, 0x10, 0x05,
	0x12, 0x0d, 0x0a, 0x09, 0x43, 0x4f, 0x4d, 0x50, 0x4c, 0x45, 0x54, 0x45, 0x44, 0x10, 0x06, 0x2a,
	0xf5, 0x01, 0x0a, 0x11, 0x43, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x4d,
	0x65, 0x74, 0x68, 0x6f, 0x64, 0x12, 0x22, 0x0a, 0x1e, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54, 0x41,
	0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x55, 0x4e, 0x53, 0x50,
	0x45, 0x43, 0x49, 0x46, 0x49, 0x45, 0x44, 0x10, 0x00, 0x12, 0x1b, 0x0a, 0x17, 0x43, 0x4f, 0x4d,
	0x50, 0x55, 0x54, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f,
	0x4d, 0x45, 0x41, 0x4e, 0x10, 0x01, 0x12, 0x1f, 0x0a, 0x1b, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54,
	0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x56, 0x41, 0x52,
	0x49, 0x41, 0x4e, 0x43, 0x45, 0x10, 0x02, 0x12, 0x1a, 0x0a, 0x16, 0x43, 0x4f, 0x4d, 0x50, 0x55,
	0x54, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x53, 0x55,
	0x4d, 0x10, 0x03, 0x12, 0x1d, 0x0a, 0x19, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54, 0x41, 0x54, 0x49,
	0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x43, 0x4f, 0x52, 0x52, 0x45, 0x4c,
	0x10, 0x04, 0x12, 0x20, 0x0a, 0x1c, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54, 0x41, 0x54, 0x49, 0x4f,
	0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x4d, 0x45, 0x53, 0x48, 0x5f, 0x43, 0x4f,
	0x44, 0x45, 0x10, 0x05, 0x12, 0x21, 0x0a, 0x1d, 0x43, 0x4f, 0x4d, 0x50, 0x55, 0x54, 0x41, 0x54,
	0x49, 0x4f, 0x4e, 0x5f, 0x4d, 0x45, 0x54, 0x48, 0x4f, 0x44, 0x5f, 0x4a, 0x4f, 0x49, 0x4e, 0x5f,
	0x54, 0x41, 0x42, 0x4c, 0x45, 0x10, 0x06, 0x2a, 0x8b, 0x01, 0x0a, 0x12, 0x53, 0x68, 0x61, 0x72,
	0x65, 0x56, 0x61, 0x6c, 0x75, 0x65, 0x54, 0x79, 0x70, 0x65, 0x45, 0x6e, 0x75, 0x6d, 0x12, 0x20,
	0x0a, 0x1c, 0x53, 0x48, 0x41, 0x52, 0x45, 0x5f, 0x56, 0x41, 0x4c, 0x55, 0x45, 0x5f, 0x54, 0x59,
	0x50, 0x45, 0x5f, 0x55, 0x4e, 0x53, 0x50, 0x45, 0x43, 0x49, 0x46, 0x49, 0x45, 0x44, 0x10, 0x00,
	0x12, 0x20, 0x0a, 0x1c, 0x53, 0x48, 0x41, 0x52, 0x45, 0x5f, 0x56, 0x41, 0x4c, 0x55, 0x45, 0x5f,
	0x54, 0x59, 0x50, 0x45, 0x5f, 0x46, 0x49, 0x58, 0x45, 0x44, 0x5f, 0x50, 0x4f, 0x49, 0x4e, 0x54,
	0x10, 0x01, 0x12, 0x31, 0x0a, 0x2d, 0x53, 0x48, 0x41, 0x52, 0x45, 0x5f, 0x56, 0x41, 0x4c, 0x55,
	0x45, 0x5f, 0x54, 0x59, 0x50, 0x45, 0x5f, 0x55, 0x54, 0x46, 0x5f, 0x38, 0x5f, 0x49, 0x4e, 0x54,
	0x45, 0x47, 0x45, 0x52, 0x5f, 0x52, 0x45, 0x50, 0x52, 0x45, 0x53, 0x45, 0x4e, 0x54, 0x41, 0x54,
	0x49, 0x4f, 0x4e, 0x10, 0x02, 0x42, 0x39, 0x5a, 0x37, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2e,
	0x63, 0x6f, 0x6d, 0x2f, 0x61, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x2d, 0x64, 0x65, 0x76,
	0x65, 0x6c, 0x6f, 0x70, 0x2f, 0x51, 0x75, 0x69, 0x63, 0x6b, 0x4d, 0x50, 0x43, 0x2f, 0x70, 0x72,
	0x6f, 0x74, 0x6f, 0x2f, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x5f, 0x74, 0x79, 0x70, 0x65, 0x73,
	0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_common_types_proto_rawDescOnce sync.Once
	file_common_types_proto_rawDescData = file_common_types_proto_rawDesc
)

func file_common_types_proto_rawDescGZIP() []byte {
	file_common_types_proto_rawDescOnce.Do(func() {
		file_common_types_proto_rawDescData = protoimpl.X.CompressGZIP(file_common_types_proto_rawDescData)
	})
	return file_common_types_proto_rawDescData
}

var file_common_types_proto_enumTypes = make([]protoimpl.EnumInfo, 3)
var file_common_types_proto_msgTypes = make([]protoimpl.MessageInfo, 6)
var file_common_types_proto_goTypes = []interface{}{
	(JobStatus)(0),            // 0: pb_common_types.JobStatus
	(ComputationMethod)(0),    // 1: pb_common_types.ComputationMethod
	(ShareValueTypeEnum)(0),   // 2: pb_common_types.ShareValueTypeEnum
	(*ProcedureProgress)(nil), // 3: pb_common_types.ProcedureProgress
	(*JobProgress)(nil),       // 4: pb_common_types.JobProgress
	(*Stacktrace)(nil),        // 5: pb_common_types.Stacktrace
	(*JobErrorInfo)(nil),      // 6: pb_common_types.JobErrorInfo
	(*Schema)(nil),            // 7: pb_common_types.Schema
	(*Stacktrace_Frame)(nil),  // 8: pb_common_types.Stacktrace.Frame
}
var file_common_types_proto_depIdxs = []int32{
	0, // 0: pb_common_types.JobProgress.status:type_name -> pb_common_types.JobStatus
	3, // 1: pb_common_types.JobProgress.progresses:type_name -> pb_common_types.ProcedureProgress
	8, // 2: pb_common_types.Stacktrace.frames:type_name -> pb_common_types.Stacktrace.Frame
	5, // 3: pb_common_types.JobErrorInfo.stacktrace:type_name -> pb_common_types.Stacktrace
	2, // 4: pb_common_types.Schema.type:type_name -> pb_common_types.ShareValueTypeEnum
	5, // [5:5] is the sub-list for method output_type
	5, // [5:5] is the sub-list for method input_type
	5, // [5:5] is the sub-list for extension type_name
	5, // [5:5] is the sub-list for extension extendee
	0, // [0:5] is the sub-list for field type_name
}

func init() { file_common_types_proto_init() }
func file_common_types_proto_init() {
	if File_common_types_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_common_types_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*ProcedureProgress); i {
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
		file_common_types_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*JobProgress); i {
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
		file_common_types_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Stacktrace); i {
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
		file_common_types_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*JobErrorInfo); i {
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
		file_common_types_proto_msgTypes[4].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Schema); i {
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
		file_common_types_proto_msgTypes[5].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Stacktrace_Frame); i {
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
	file_common_types_proto_msgTypes[0].OneofWrappers = []interface{}{}
	file_common_types_proto_msgTypes[3].OneofWrappers = []interface{}{}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_common_types_proto_rawDesc,
			NumEnums:      3,
			NumMessages:   6,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_common_types_proto_goTypes,
		DependencyIndexes: file_common_types_proto_depIdxs,
		EnumInfos:         file_common_types_proto_enumTypes,
		MessageInfos:      file_common_types_proto_msgTypes,
	}.Build()
	File_common_types_proto = out.File
	file_common_types_proto_rawDesc = nil
	file_common_types_proto_goTypes = nil
	file_common_types_proto_depIdxs = nil
}
