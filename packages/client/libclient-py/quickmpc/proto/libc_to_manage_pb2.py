# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: libc_to_manage.proto
"""Generated protocol buffer code."""
from google.protobuf import empty_pb2 as google_dot_protobuf_dot_empty__pb2
from common_types import common_types_pb2 as common__types_dot_common__types__pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x14libc_to_manage.proto\x12\x0clibctomanage\x1a\x1f\x63ommon_types/common_types.proto\x1a\x1bgoogle/protobuf/empty.proto\"\xa8\x01\n\x11SendSharesRequest\x12\x0f\n\x07\x64\x61ta_id\x18\x01 \x01(\t\x12\x0e\n\x06shares\x18\x02 \x01(\t\x12\'\n\x06schema\x18\x03 \x03(\x0b\x32\x17.pb_common_types.Schema\x12\x10\n\x08piece_id\x18\x04 \x01(\x05\x12\x0f\n\x07sent_at\x18\x05 \x01(\t\x12\x17\n\x0fmatching_column\x18\x06 \x01(\x05\x12\r\n\x05token\x18\x07 \x01(\t\"5\n\x13\x44\x65leteSharesRequest\x12\x0f\n\x07\x64\x61taIds\x18\x01 \x03(\t\x12\r\n\x05token\x18\x02 \x01(\t\"2\n\x10GetSchemaRequest\x12\x0f\n\x07\x64\x61ta_id\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"<\n\x11GetSchemaResponse\x12\'\n\x06schema\x18\x01 \x03(\x0b\x32\x17.pb_common_types.Schema\"1\n\tJoinOrder\x12\x10\n\x08\x64\x61ta_ids\x18\x01 \x03(\t\x12\x12\n\ndebug_mode\x18\x02 \x01(\x08\"$\n\x05Input\x12\x0b\n\x03src\x18\x01 \x03(\x05\x12\x0e\n\x06target\x18\x02 \x03(\x05\"\xab\x01\n\x19\x45xecuteComputationRequest\x12\x35\n\tmethod_id\x18\x01 \x01(\x0e\x32\".pb_common_types.ComputationMethod\x12\r\n\x05token\x18\x02 \x01(\t\x12&\n\x05table\x18\x03 \x01(\x0b\x32\x17.libctomanage.JoinOrder\x12 \n\x03\x61rg\x18\x04 \x01(\x0b\x32\x13.libctomanage.Input\".\n\x1a\x45xecuteComputationResponse\x12\x10\n\x08job_uuid\x18\x03 \x01(\t\">\n\x1bGetComputationResultRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"\x8f\x02\n\x1cGetComputationResultResponse\x12\x0e\n\x06result\x18\x03 \x03(\t\x12\x15\n\rcolumn_number\x18\x04 \x01(\x05\x12*\n\x06status\x18\x05 \x01(\x0e\x32\x1a.pb_common_types.JobStatus\x12\x10\n\x08piece_id\x18\x06 \x01(\x05\x12\x33\n\x08progress\x18\x07 \x01(\x0b\x32\x1c.pb_common_types.JobProgressH\x01\x88\x01\x01\x12\x11\n\x07is_dim1\x18\x08 \x01(\x08H\x00\x12\x11\n\x07is_dim2\x18\t \x01(\x08H\x00\x12\x13\n\tis_schema\x18\n \x01(\x08H\x00\x42\r\n\x0bresult_typeB\x0b\n\t_progress\"#\n\x12GetDataListRequest\x12\r\n\x05token\x18\x01 \x01(\t\"%\n\x13GetDataListResponse\x12\x0e\n\x06result\x18\x01 \x01(\t\"8\n\x15GetElapsedTimeRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\".\n\x16GetElapsedTimeResponse\x12\x14\n\x0c\x65lapsed_time\x18\x01 \x01(\x01\"9\n\x16GetJobErrorInfoRequest\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12\r\n\x05token\x18\x02 \x01(\t\"h\n\x17GetJobErrorInfoResponse\x12:\n\x0ejob_error_info\x18\x01 \x01(\x0b\x32\x1d.pb_common_types.JobErrorInfoH\x00\x88\x01\x01\x42\x11\n\x0f_job_error_info\"D\n\x13\x41\x64\x64ValueToIdRequest\x12\x0f\n\x07\x64\x61ta_id\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x03(\t\x12\r\n\x05token\x18\x03 \x01(\t2\xb6\x06\n\x0cLibcToManage\x12G\n\nSendShares\x12\x1f.libctomanage.SendSharesRequest\x1a\x16.google.protobuf.Empty\"\x00\x12K\n\x0c\x44\x65leteShares\x12!.libctomanage.DeleteSharesRequest\x1a\x16.google.protobuf.Empty\"\x00\x12N\n\tGetSchema\x12\x1e.libctomanage.GetSchemaRequest\x1a\x1f.libctomanage.GetSchemaResponse\"\x00\x12i\n\x12\x45xecuteComputation\x12\'.libctomanage.ExecuteComputationRequest\x1a(.libctomanage.ExecuteComputationResponse\"\x00\x12q\n\x14GetComputationResult\x12).libctomanage.GetComputationResultRequest\x1a*.libctomanage.GetComputationResultResponse\"\x00\x30\x01\x12T\n\x0bGetDataList\x12 .libctomanage.GetDataListRequest\x1a!.libctomanage.GetDataListResponse\"\x00\x12]\n\x0eGetElapsedTime\x12#.libctomanage.GetElapsedTimeRequest\x1a$.libctomanage.GetElapsedTimeResponse\"\x00\x12`\n\x0fGetJobErrorInfo\x12$.libctomanage.GetJobErrorInfoRequest\x1a%.libctomanage.GetJobErrorInfoResponse\"\x00\x12K\n\x0c\x41\x64\x64ValueToId\x12!.libctomanage.AddValueToIdRequest\x1a\x16.google.protobuf.Empty\"\x00\x42\x45ZCgithub.com/acompany-develop/QuickMPC/proto/libc_to_manage_containerb\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(
    DESCRIPTOR, 'libc_to_manage_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:

    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'ZCgithub.com/acompany-develop/QuickMPC/proto/libc_to_manage_container'
    _globals['_SENDSHARESREQUEST']._serialized_start = 101
    _globals['_SENDSHARESREQUEST']._serialized_end = 269
    _globals['_DELETESHARESREQUEST']._serialized_start = 271
    _globals['_DELETESHARESREQUEST']._serialized_end = 324
    _globals['_GETSCHEMAREQUEST']._serialized_start = 326
    _globals['_GETSCHEMAREQUEST']._serialized_end = 376
    _globals['_GETSCHEMARESPONSE']._serialized_start = 378
    _globals['_GETSCHEMARESPONSE']._serialized_end = 438
    _globals['_JOINORDER']._serialized_start = 440
    _globals['_JOINORDER']._serialized_end = 489
    _globals['_INPUT']._serialized_start = 491
    _globals['_INPUT']._serialized_end = 527
    _globals['_EXECUTECOMPUTATIONREQUEST']._serialized_start = 530
    _globals['_EXECUTECOMPUTATIONREQUEST']._serialized_end = 701
    _globals['_EXECUTECOMPUTATIONRESPONSE']._serialized_start = 703
    _globals['_EXECUTECOMPUTATIONRESPONSE']._serialized_end = 749
    _globals['_GETCOMPUTATIONRESULTREQUEST']._serialized_start = 751
    _globals['_GETCOMPUTATIONRESULTREQUEST']._serialized_end = 813
    _globals['_GETCOMPUTATIONRESULTRESPONSE']._serialized_start = 816
    _globals['_GETCOMPUTATIONRESULTRESPONSE']._serialized_end = 1087
    _globals['_GETDATALISTREQUEST']._serialized_start = 1089
    _globals['_GETDATALISTREQUEST']._serialized_end = 1124
    _globals['_GETDATALISTRESPONSE']._serialized_start = 1126
    _globals['_GETDATALISTRESPONSE']._serialized_end = 1163
    _globals['_GETELAPSEDTIMEREQUEST']._serialized_start = 1165
    _globals['_GETELAPSEDTIMEREQUEST']._serialized_end = 1221
    _globals['_GETELAPSEDTIMERESPONSE']._serialized_start = 1223
    _globals['_GETELAPSEDTIMERESPONSE']._serialized_end = 1269
    _globals['_GETJOBERRORINFOREQUEST']._serialized_start = 1271
    _globals['_GETJOBERRORINFOREQUEST']._serialized_end = 1328
    _globals['_GETJOBERRORINFORESPONSE']._serialized_start = 1330
    _globals['_GETJOBERRORINFORESPONSE']._serialized_end = 1434
    _globals['_ADDVALUETOIDREQUEST']._serialized_start = 1436
    _globals['_ADDVALUETOIDREQUEST']._serialized_end = 1504
    _globals['_LIBCTOMANAGE']._serialized_start = 1507
    _globals['_LIBCTOMANAGE']._serialized_end = 2329
# @@protoc_insertion_point(module_scope)
