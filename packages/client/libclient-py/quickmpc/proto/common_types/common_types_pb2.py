# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: common_types/common_types.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x1f\x63ommon_types/common_types.proto\x12\x0fpb_common_types\"{\n\x11ProcedureProgress\x12\n\n\x02id\x18\x01 \x01(\x04\x12\x13\n\x0b\x64\x65scription\x18\x02 \x01(\t\x12\x10\n\x08progress\x18\x03 \x01(\x02\x12\x11\n\tcompleted\x18\x04 \x01(\x08\x12\x14\n\x07\x64\x65tails\x18\x05 \x01(\tH\x00\x88\x01\x01\x42\n\n\x08_details\"\x83\x01\n\x0bJobProgress\x12\x10\n\x08job_uuid\x18\x01 \x01(\t\x12*\n\x06status\x18\x02 \x01(\x0e\x32\x1a.pb_common_types.JobStatus\x12\x36\n\nprogresses\x18\x03 \x03(\x0b\x32\".pb_common_types.ProcedureProgress\"\x8d\x01\n\nStacktrace\x12\x31\n\x06\x66rames\x18\x01 \x03(\x0b\x32!.pb_common_types.Stacktrace.Frame\x1aL\n\x05\x46rame\x12\x17\n\x0fsource_location\x18\x01 \x01(\t\x12\x13\n\x0bsource_line\x18\x02 \x01(\x04\x12\x15\n\rfunction_name\x18\x03 \x01(\t\"\x93\x01\n\x0cJobErrorInfo\x12\x0c\n\x04what\x18\x01 \x01(\t\x12\x1c\n\x0f\x61\x64\x64itional_info\x18\x02 \x01(\tH\x00\x88\x01\x01\x12\x34\n\nstacktrace\x18\x03 \x01(\x0b\x32\x1b.pb_common_types.StacktraceH\x01\x88\x01\x01\x42\x12\n\x10_additional_infoB\r\n\x0b_stacktrace\"I\n\x06Schema\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x31\n\x04type\x18\x02 \x01(\x0e\x32#.pb_common_types.ShareValueTypeEnum\"+\n\nBigIntByte\x12\x0b\n\x03sgn\x18\x01 \x01(\x08\x12\x10\n\x08\x61\x62s_byte\x18\x02 \x01(\x0c*g\n\tJobStatus\x12\x0b\n\x07UNKNOWN\x10\x00\x12\t\n\x05\x45RROR\x10\x01\x12\x0c\n\x08RECEIVED\x10\x02\x12\x0b\n\x07PRE_JOB\x10\x03\x12\x0b\n\x07READ_DB\x10\x04\x12\x0b\n\x07\x43OMPUTE\x10\x05\x12\r\n\tCOMPLETED\x10\x06*\xf5\x01\n\x11\x43omputationMethod\x12\"\n\x1e\x43OMPUTATION_METHOD_UNSPECIFIED\x10\x00\x12\x1b\n\x17\x43OMPUTATION_METHOD_MEAN\x10\x01\x12\x1f\n\x1b\x43OMPUTATION_METHOD_VARIANCE\x10\x02\x12\x1a\n\x16\x43OMPUTATION_METHOD_SUM\x10\x03\x12\x1d\n\x19\x43OMPUTATION_METHOD_CORREL\x10\x04\x12 \n\x1c\x43OMPUTATION_METHOD_MESH_CODE\x10\x05\x12!\n\x1d\x43OMPUTATION_METHOD_JOIN_TABLE\x10\x06*\x8b\x01\n\x12ShareValueTypeEnum\x12 \n\x1cSHARE_VALUE_TYPE_UNSPECIFIED\x10\x00\x12 \n\x1cSHARE_VALUE_TYPE_FIXED_POINT\x10\x01\x12\x31\n-SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION\x10\x02\x42\x39Z7github.com/acompany-develop/QuickMPC/proto/common_typesb\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(
    DESCRIPTOR, 'common_types.common_types_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:

    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'Z7github.com/acompany-develop/QuickMPC/proto/common_types'
    _globals['_JOBSTATUS']._serialized_start = 725
    _globals['_JOBSTATUS']._serialized_end = 828
    _globals['_COMPUTATIONMETHOD']._serialized_start = 831
    _globals['_COMPUTATIONMETHOD']._serialized_end = 1076
    _globals['_SHAREVALUETYPEENUM']._serialized_start = 1079
    _globals['_SHAREVALUETYPEENUM']._serialized_end = 1218
    _globals['_PROCEDUREPROGRESS']._serialized_start = 52
    _globals['_PROCEDUREPROGRESS']._serialized_end = 175
    _globals['_JOBPROGRESS']._serialized_start = 178
    _globals['_JOBPROGRESS']._serialized_end = 309
    _globals['_STACKTRACE']._serialized_start = 312
    _globals['_STACKTRACE']._serialized_end = 453
    _globals['_STACKTRACE_FRAME']._serialized_start = 377
    _globals['_STACKTRACE_FRAME']._serialized_end = 453
    _globals['_JOBERRORINFO']._serialized_start = 456
    _globals['_JOBERRORINFO']._serialized_end = 603
    _globals['_SCHEMA']._serialized_start = 605
    _globals['_SCHEMA']._serialized_end = 678
    _globals['_BIGINTBYTE']._serialized_start = 680
    _globals['_BIGINTBYTE']._serialized_end = 723
# @@protoc_insertion_point(module_scope)
