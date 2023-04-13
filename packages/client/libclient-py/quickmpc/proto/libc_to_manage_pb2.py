# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: libc_to_manage.proto
"""Generated protocol buffer code."""
from google.protobuf import empty_pb2 as google_dot_protobuf_dot_empty__pb2
from common_types import common_types_pb2 as common__types_dot_common__types__pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x14libc_to_manage.proto\x12\x0clibctomanage\x1a\x1f\x63ommon_types/common_types.proto\x1a\x1bgoogle/protobuf/empty.proto\"\xa8\x01\n\x11SendSharesRequest\x12\x0f\n\x07\x64\x61ta_id\x18\x01 \x01(\t\x12\x0e\n\x06shares\x18\x02 \x01(\t\x12\'\n\x06schema\x18\x03 \x03(\x0b\x32\x17.pb_common_types.Schema\x12\x10\n\x08piece_id\x18\x04 \x01(\x05\x12\x0f\n\x07sent_at\x18\x05 \x01(\t\x12\x17\n\x0fmatching_column\x18\x06 \x01(\x05\x12\r\n\x05token\x18\x07 \x01(\t\"5\n\x13\x44\x65leteSharesRequest\x12\x0f\n\x07\x64\x61taIds\x18\x01 \x03(\t\x12\r\n\x05token\x18\x02 \x01(\t\"2\n\x10GetSchemaRequest\x12\x0f\n\x07\x64\x61ta_id\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"<\n\x11GetSchemaResponse\x12\'\n\x06schema\x18\x01 \x03(\x0b\x32\x17.pb_common_types.Schema\"9\n\tJoinOrder\x12\x0f\n\x07\x64\x61taIds\x18\x01 \x03(\t\x12\x0c\n\x04join\x18\x02 \x03(\x05\x12\r\n\x05index\x18\x03 \x03(\x05\"$\n\x05Input\x12\x0b\n\x03src\x18\x01 \x03(\x05\x12\x0e\n\x06target\x18\x02 \x03(\x05\"\xab\x01\n\x19\x45xecuteComputationRequest\x12\x35\n\tmethod_id\x18\x01 \x01(\x0e\x32\".pb_common_types.ComputationMethod\x12\r\n\x05token\x18\x02 \x01(\t\x12&\n\x05table\x18\x03 \x01(\x0b\x32\x17.libctomanage.JoinOrder\x12 \n\x03\x61rg\x18\x04 \x01(\x0b\x32\x13.libctomanage.Input\".\n\x1a\x45xecuteComputationResponse\x12\x10\n\x08job_uuid\x18\x03 \x01(\t\">\n\x1bGetComputationResultRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"\x8f\x02\n\x1cGetComputationResultResponse\x12\x0e\n\x06result\x18\x03 \x03(\t\x12\x15\n\rcolumn_number\x18\x04 \x01(\x05\x12*\n\x06status\x18\x05 \x01(\x0e\x32\x1a.pb_common_types.JobStatus\x12\x10\n\x08piece_id\x18\x06 \x01(\x05\x12\x33\n\x08progress\x18\x07 \x01(\x0b\x32\x1c.pb_common_types.JobProgressH\x01\x88\x01\x01\x12\x11\n\x07is_dim1\x18\x08 \x01(\x08H\x00\x12\x11\n\x07is_dim2\x18\t \x01(\x08H\x00\x12\x13\n\tis_schema\x18\n \x01(\x08H\x00\x42\r\n\x0bresult_typeB\x0b\n\t_progress\"#\n\x12GetDataListRequest\x12\r\n\x05token\x18\x01 \x01(\t\"%\n\x13GetDataListResponse\x12\x0e\n\x06result\x18\x01 \x01(\t\"8\n\x15GetElapsedTimeRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\".\n\x16GetElapsedTimeResponse\x12\x14\n\x0c\x65lapsed_time\x18\x01 \x01(\x01\"9\n\x16GetJobErrorInfoRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"h\n\x17GetJobErrorInfoResponse\x12:\n\x0ejob_error_info\x18\x01 \x01(\x0b\x32\x1d.pb_common_types.JobErrorInfoH\x00\x88\x01\x01\x42\x11\n\x0f_job_error_info2\xe9\x05\n\x0cLibcToManage\x12G\n\nSendShares\x12\x1f.libctomanage.SendSharesRequest\x1a\x16.google.protobuf.Empty\"\x00\x12K\n\x0c\x44\x65leteShares\x12!.libctomanage.DeleteSharesRequest\x1a\x16.google.protobuf.Empty\"\x00\x12N\n\tGetSchema\x12\x1e.libctomanage.GetSchemaRequest\x1a\x1f.libctomanage.GetSchemaResponse\"\x00\x12i\n\x12\x45xecuteComputation\x12\'.libctomanage.ExecuteComputationRequest\x1a(.libctomanage.ExecuteComputationResponse\"\x00\x12q\n\x14GetComputationResult\x12).libctomanage.GetComputationResultRequest\x1a*.libctomanage.GetComputationResultResponse\"\x00\x30\x01\x12T\n\x0bGetDataList\x12 .libctomanage.GetDataListRequest\x1a!.libctomanage.GetDataListResponse\"\x00\x12]\n\x0eGetElapsedTime\x12#.libctomanage.GetElapsedTimeRequest\x1a$.libctomanage.GetElapsedTimeResponse\"\x00\x12`\n\x0fGetJobErrorInfo\x12$.libctomanage.GetJobErrorInfoRequest\x1a%.libctomanage.GetJobErrorInfoResponse\"\x00\x42\x45ZCgithub.com/acompany-develop/QuickMPC/proto/libc_to_manage_containerb\x06proto3')


_SENDSHARESREQUEST = DESCRIPTOR.message_types_by_name['SendSharesRequest']
_DELETESHARESREQUEST = DESCRIPTOR.message_types_by_name['DeleteSharesRequest']
_GETSCHEMAREQUEST = DESCRIPTOR.message_types_by_name['GetSchemaRequest']
_GETSCHEMARESPONSE = DESCRIPTOR.message_types_by_name['GetSchemaResponse']
_JOINORDER = DESCRIPTOR.message_types_by_name['JoinOrder']
_INPUT = DESCRIPTOR.message_types_by_name['Input']
_EXECUTECOMPUTATIONREQUEST = DESCRIPTOR.message_types_by_name['ExecuteComputationRequest']
_EXECUTECOMPUTATIONRESPONSE = DESCRIPTOR.message_types_by_name['ExecuteComputationResponse']
_GETCOMPUTATIONRESULTREQUEST = DESCRIPTOR.message_types_by_name['GetComputationResultRequest']
_GETCOMPUTATIONRESULTRESPONSE = DESCRIPTOR.message_types_by_name['GetComputationResultResponse']
_GETDATALISTREQUEST = DESCRIPTOR.message_types_by_name['GetDataListRequest']
_GETDATALISTRESPONSE = DESCRIPTOR.message_types_by_name['GetDataListResponse']
_GETELAPSEDTIMEREQUEST = DESCRIPTOR.message_types_by_name['GetElapsedTimeRequest']
_GETELAPSEDTIMERESPONSE = DESCRIPTOR.message_types_by_name['GetElapsedTimeResponse']
_GETJOBERRORINFOREQUEST = DESCRIPTOR.message_types_by_name['GetJobErrorInfoRequest']
_GETJOBERRORINFORESPONSE = DESCRIPTOR.message_types_by_name['GetJobErrorInfoResponse']
SendSharesRequest = _reflection.GeneratedProtocolMessageType('SendSharesRequest', (_message.Message,), {
    'DESCRIPTOR': _SENDSHARESREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.SendSharesRequest)
})
_sym_db.RegisterMessage(SendSharesRequest)

DeleteSharesRequest = _reflection.GeneratedProtocolMessageType('DeleteSharesRequest', (_message.Message,), {
    'DESCRIPTOR': _DELETESHARESREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.DeleteSharesRequest)
})
_sym_db.RegisterMessage(DeleteSharesRequest)

GetSchemaRequest = _reflection.GeneratedProtocolMessageType('GetSchemaRequest', (_message.Message,), {
    'DESCRIPTOR': _GETSCHEMAREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetSchemaRequest)
})
_sym_db.RegisterMessage(GetSchemaRequest)

GetSchemaResponse = _reflection.GeneratedProtocolMessageType('GetSchemaResponse', (_message.Message,), {
    'DESCRIPTOR': _GETSCHEMARESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetSchemaResponse)
})
_sym_db.RegisterMessage(GetSchemaResponse)

JoinOrder = _reflection.GeneratedProtocolMessageType('JoinOrder', (_message.Message,), {
    'DESCRIPTOR': _JOINORDER,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.JoinOrder)
})
_sym_db.RegisterMessage(JoinOrder)

Input = _reflection.GeneratedProtocolMessageType('Input', (_message.Message,), {
    'DESCRIPTOR': _INPUT,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.Input)
})
_sym_db.RegisterMessage(Input)

ExecuteComputationRequest = _reflection.GeneratedProtocolMessageType('ExecuteComputationRequest', (_message.Message,), {
    'DESCRIPTOR': _EXECUTECOMPUTATIONREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.ExecuteComputationRequest)
})
_sym_db.RegisterMessage(ExecuteComputationRequest)

ExecuteComputationResponse = _reflection.GeneratedProtocolMessageType('ExecuteComputationResponse', (_message.Message,), {
    'DESCRIPTOR': _EXECUTECOMPUTATIONRESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.ExecuteComputationResponse)
})
_sym_db.RegisterMessage(ExecuteComputationResponse)

GetComputationResultRequest = _reflection.GeneratedProtocolMessageType('GetComputationResultRequest', (_message.Message,), {
    'DESCRIPTOR': _GETCOMPUTATIONRESULTREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetComputationResultRequest)
})
_sym_db.RegisterMessage(GetComputationResultRequest)

GetComputationResultResponse = _reflection.GeneratedProtocolMessageType('GetComputationResultResponse', (_message.Message,), {
    'DESCRIPTOR': _GETCOMPUTATIONRESULTRESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetComputationResultResponse)
})
_sym_db.RegisterMessage(GetComputationResultResponse)

GetDataListRequest = _reflection.GeneratedProtocolMessageType('GetDataListRequest', (_message.Message,), {
    'DESCRIPTOR': _GETDATALISTREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetDataListRequest)
})
_sym_db.RegisterMessage(GetDataListRequest)

GetDataListResponse = _reflection.GeneratedProtocolMessageType('GetDataListResponse', (_message.Message,), {
    'DESCRIPTOR': _GETDATALISTRESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetDataListResponse)
})
_sym_db.RegisterMessage(GetDataListResponse)

GetElapsedTimeRequest = _reflection.GeneratedProtocolMessageType('GetElapsedTimeRequest', (_message.Message,), {
    'DESCRIPTOR': _GETELAPSEDTIMEREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetElapsedTimeRequest)
})
_sym_db.RegisterMessage(GetElapsedTimeRequest)

GetElapsedTimeResponse = _reflection.GeneratedProtocolMessageType('GetElapsedTimeResponse', (_message.Message,), {
    'DESCRIPTOR': _GETELAPSEDTIMERESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetElapsedTimeResponse)
})
_sym_db.RegisterMessage(GetElapsedTimeResponse)

GetJobErrorInfoRequest = _reflection.GeneratedProtocolMessageType('GetJobErrorInfoRequest', (_message.Message,), {
    'DESCRIPTOR': _GETJOBERRORINFOREQUEST,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetJobErrorInfoRequest)
})
_sym_db.RegisterMessage(GetJobErrorInfoRequest)

GetJobErrorInfoResponse = _reflection.GeneratedProtocolMessageType('GetJobErrorInfoResponse', (_message.Message,), {
    'DESCRIPTOR': _GETJOBERRORINFORESPONSE,
    '__module__': 'libc_to_manage_pb2'
    # @@protoc_insertion_point(class_scope:libctomanage.GetJobErrorInfoResponse)
})
_sym_db.RegisterMessage(GetJobErrorInfoResponse)

_LIBCTOMANAGE = DESCRIPTOR.services_by_name['LibcToManage']
if _descriptor._USE_C_DESCRIPTORS == False:

    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'ZCgithub.com/acompany-develop/QuickMPC/proto/libc_to_manage_container'
    _SENDSHARESREQUEST._serialized_start = 101
    _SENDSHARESREQUEST._serialized_end = 269
    _DELETESHARESREQUEST._serialized_start = 271
    _DELETESHARESREQUEST._serialized_end = 324
    _GETSCHEMAREQUEST._serialized_start = 326
    _GETSCHEMAREQUEST._serialized_end = 376
    _GETSCHEMARESPONSE._serialized_start = 378
    _GETSCHEMARESPONSE._serialized_end = 438
    _JOINORDER._serialized_start = 440
    _JOINORDER._serialized_end = 497
    _INPUT._serialized_start = 499
    _INPUT._serialized_end = 535
    _EXECUTECOMPUTATIONREQUEST._serialized_start = 538
    _EXECUTECOMPUTATIONREQUEST._serialized_end = 709
    _EXECUTECOMPUTATIONRESPONSE._serialized_start = 711
    _EXECUTECOMPUTATIONRESPONSE._serialized_end = 757
    _GETCOMPUTATIONRESULTREQUEST._serialized_start = 759
    _GETCOMPUTATIONRESULTREQUEST._serialized_end = 821
    _GETCOMPUTATIONRESULTRESPONSE._serialized_start = 824
    _GETCOMPUTATIONRESULTRESPONSE._serialized_end = 1095
    _GETDATALISTREQUEST._serialized_start = 1097
    _GETDATALISTREQUEST._serialized_end = 1132
    _GETDATALISTRESPONSE._serialized_start = 1134
    _GETDATALISTRESPONSE._serialized_end = 1171
    _GETELAPSEDTIMEREQUEST._serialized_start = 1173
    _GETELAPSEDTIMEREQUEST._serialized_end = 1229
    _GETELAPSEDTIMERESPONSE._serialized_start = 1231
    _GETELAPSEDTIMERESPONSE._serialized_end = 1277
    _GETJOBERRORINFOREQUEST._serialized_start = 1279
    _GETJOBERRORINFOREQUEST._serialized_end = 1336
    _GETJOBERRORINFORESPONSE._serialized_start = 1338
    _GETJOBERRORINFORESPONSE._serialized_end = 1442
    _LIBCTOMANAGE._serialized_start = 1445
    _LIBCTOMANAGE._serialized_end = 2190
# @@protoc_insertion_point(module_scope)
