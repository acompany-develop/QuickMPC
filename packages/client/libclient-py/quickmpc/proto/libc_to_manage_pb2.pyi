"""
@generated by mypy-protobuf.  Do not edit manually!
isort:skip_file
"""
import builtins
import collections.abc
import common_types.common_types_pb2
import google.protobuf.descriptor
import google.protobuf.internal.containers
import google.protobuf.message
import sys

if sys.version_info >= (3, 8):
    import typing as typing_extensions
else:
    import typing_extensions

DESCRIPTOR: google.protobuf.descriptor.FileDescriptor

class SendSharesRequest(google.protobuf.message.Message):
    """*
    the message of SendSharesRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATA_ID_FIELD_NUMBER: builtins.int
    SHARES_FIELD_NUMBER: builtins.int
    SCHEMA_FIELD_NUMBER: builtins.int
    PIECE_ID_FIELD_NUMBER: builtins.int
    SENT_AT_FIELD_NUMBER: builtins.int
    MATCHING_COLUMN_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    data_id: builtins.str
    shares: builtins.str
    @property
    def schema(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[common_types.common_types_pb2.Schema]: ...
    piece_id: builtins.int
    sent_at: builtins.str
    matching_column: builtins.int
    token: builtins.str
    def __init__(
        self,
        *,
        data_id: builtins.str = ...,
        shares: builtins.str = ...,
        schema: collections.abc.Iterable[common_types.common_types_pb2.Schema] | None = ...,
        piece_id: builtins.int = ...,
        sent_at: builtins.str = ...,
        matching_column: builtins.int = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["data_id", b"data_id", "matching_column", b"matching_column", "piece_id", b"piece_id", "schema", b"schema", "sent_at", b"sent_at", "shares", b"shares", "token", b"token"]) -> None: ...

global___SendSharesRequest = SendSharesRequest

class DeleteSharesRequest(google.protobuf.message.Message):
    """*
    the message of DeleteSharesRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATAIDS_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    @property
    def dataIds(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.str]: ...
    token: builtins.str
    def __init__(
        self,
        *,
        dataIds: collections.abc.Iterable[builtins.str] | None = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["dataIds", b"dataIds", "token", b"token"]) -> None: ...

global___DeleteSharesRequest = DeleteSharesRequest

class GetSchemaRequest(google.protobuf.message.Message):
    """*
    the message of GetSchemaRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATA_ID_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    data_id: builtins.str
    token: builtins.str
    def __init__(
        self,
        *,
        data_id: builtins.str = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["data_id", b"data_id", "token", b"token"]) -> None: ...

global___GetSchemaRequest = GetSchemaRequest

class GetSchemaResponse(google.protobuf.message.Message):
    """*
    the message of GetSchemaResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    SCHEMA_FIELD_NUMBER: builtins.int
    @property
    def schema(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[common_types.common_types_pb2.Schema]: ...
    def __init__(
        self,
        *,
        schema: collections.abc.Iterable[common_types.common_types_pb2.Schema] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["schema", b"schema"]) -> None: ...

global___GetSchemaResponse = GetSchemaResponse

class JoinOrder(google.protobuf.message.Message):
    """*
    the message of ExecuteComputationRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATA_IDS_FIELD_NUMBER: builtins.int
    DEBUG_MODE_FIELD_NUMBER: builtins.int
    @property
    def data_ids(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.str]: ...
    debug_mode: builtins.bool
    def __init__(
        self,
        *,
        data_ids: collections.abc.Iterable[builtins.str] | None = ...,
        debug_mode: builtins.bool = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["data_ids", b"data_ids", "debug_mode", b"debug_mode"]) -> None: ...

global___JoinOrder = JoinOrder

class Input(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    SRC_FIELD_NUMBER: builtins.int
    TARGET_FIELD_NUMBER: builtins.int
    @property
    def src(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.int]: ...
    @property
    def target(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.int]: ...
    def __init__(
        self,
        *,
        src: collections.abc.Iterable[builtins.int] | None = ...,
        target: collections.abc.Iterable[builtins.int] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["src", b"src", "target", b"target"]) -> None: ...

global___Input = Input

class ExecuteComputationRequest(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    METHOD_ID_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    TABLE_FIELD_NUMBER: builtins.int
    ARG_FIELD_NUMBER: builtins.int
    method_id: common_types.common_types_pb2.ComputationMethod.ValueType
    token: builtins.str
    @property
    def table(self) -> global___JoinOrder: ...
    @property
    def arg(self) -> global___Input: ...
    def __init__(
        self,
        *,
        method_id: common_types.common_types_pb2.ComputationMethod.ValueType = ...,
        token: builtins.str = ...,
        table: global___JoinOrder | None = ...,
        arg: global___Input | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["arg", b"arg", "table", b"table"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["arg", b"arg", "method_id", b"method_id", "table", b"table", "token", b"token"]) -> None: ...

global___ExecuteComputationRequest = ExecuteComputationRequest

class ExecuteComputationResponse(google.protobuf.message.Message):
    """
    the message of ExecuteComputationResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_UUID_FIELD_NUMBER: builtins.int
    job_uuid: builtins.str
    def __init__(
        self,
        *,
        job_uuid: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["job_uuid", b"job_uuid"]) -> None: ...

global___ExecuteComputationResponse = ExecuteComputationResponse

class GetComputationRequest(google.protobuf.message.Message):
    """*
    the message of GetComputationResultRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_UUID_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    job_uuid: builtins.str
    token: builtins.str
    def __init__(
        self,
        *,
        job_uuid: builtins.str = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["job_uuid", b"job_uuid", "token", b"token"]) -> None: ...

global___GetComputationRequest = GetComputationRequest

class GetComputationResultResponse(google.protobuf.message.Message):
    """*
    the message of GetComputationResultResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    RESULT_FIELD_NUMBER: builtins.int
    COLUMN_NUMBER_FIELD_NUMBER: builtins.int
    PIECE_ID_FIELD_NUMBER: builtins.int
    IS_DIM1_FIELD_NUMBER: builtins.int
    IS_DIM2_FIELD_NUMBER: builtins.int
    IS_SCHEMA_FIELD_NUMBER: builtins.int
    @property
    def result(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.str]: ...
    column_number: builtins.int
    piece_id: builtins.int
    is_dim1: builtins.bool
    is_dim2: builtins.bool
    is_schema: builtins.bool
    def __init__(
        self,
        *,
        result: collections.abc.Iterable[builtins.str] | None = ...,
        column_number: builtins.int = ...,
        piece_id: builtins.int = ...,
        is_dim1: builtins.bool = ...,
        is_dim2: builtins.bool = ...,
        is_schema: builtins.bool = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["is_dim1", b"is_dim1", "is_dim2", b"is_dim2", "is_schema", b"is_schema", "result_type", b"result_type"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["column_number", b"column_number", "is_dim1", b"is_dim1", "is_dim2", b"is_dim2", "is_schema", b"is_schema", "piece_id", b"piece_id", "result", b"result", "result_type", b"result_type"]) -> None: ...
    def WhichOneof(self, oneof_group: typing_extensions.Literal["result_type", b"result_type"]) -> typing_extensions.Literal["is_dim1", "is_dim2", "is_schema"] | None: ...

global___GetComputationResultResponse = GetComputationResultResponse

class GetComputationStatusResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    STATUS_FIELD_NUMBER: builtins.int
    PROGRESS_FIELD_NUMBER: builtins.int
    status: common_types.common_types_pb2.JobStatus.ValueType
    @property
    def progress(self) -> common_types.common_types_pb2.JobProgress: ...
    def __init__(
        self,
        *,
        status: common_types.common_types_pb2.JobStatus.ValueType = ...,
        progress: common_types.common_types_pb2.JobProgress | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_progress", b"_progress", "progress", b"progress"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_progress", b"_progress", "progress", b"progress", "status", b"status"]) -> None: ...
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_progress", b"_progress"]) -> typing_extensions.Literal["progress"] | None: ...

global___GetComputationStatusResponse = GetComputationStatusResponse

class GetElapsedTimeRequest(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_UUID_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    job_uuid: builtins.str
    token: builtins.str
    def __init__(
        self,
        *,
        job_uuid: builtins.str = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["job_uuid", b"job_uuid", "token", b"token"]) -> None: ...

global___GetElapsedTimeRequest = GetElapsedTimeRequest

class GetElapsedTimeResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    ELAPSED_TIME_FIELD_NUMBER: builtins.int
    elapsed_time: builtins.float
    def __init__(
        self,
        *,
        elapsed_time: builtins.float = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["elapsed_time", b"elapsed_time"]) -> None: ...

global___GetElapsedTimeResponse = GetElapsedTimeResponse

class GetJobErrorInfoResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_ERROR_INFO_FIELD_NUMBER: builtins.int
    @property
    def job_error_info(self) -> common_types.common_types_pb2.JobErrorInfo: ...
    def __init__(
        self,
        *,
        job_error_info: common_types.common_types_pb2.JobErrorInfo | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_job_error_info", b"_job_error_info", "job_error_info", b"job_error_info"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_job_error_info", b"_job_error_info", "job_error_info", b"job_error_info"]) -> None: ...
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_job_error_info", b"_job_error_info"]) -> typing_extensions.Literal["job_error_info"] | None: ...

global___GetJobErrorInfoResponse = GetJobErrorInfoResponse

class AddShareDataFrameRequest(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    BASE_DATA_ID_FIELD_NUMBER: builtins.int
    ADD_DATA_ID_FIELD_NUMBER: builtins.int
    TOKEN_FIELD_NUMBER: builtins.int
    base_data_id: builtins.str
    add_data_id: builtins.str
    token: builtins.str
    def __init__(
        self,
        *,
        base_data_id: builtins.str = ...,
        add_data_id: builtins.str = ...,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["add_data_id", b"add_data_id", "base_data_id", b"base_data_id", "token", b"token"]) -> None: ...

global___AddShareDataFrameRequest = AddShareDataFrameRequest

class AddShareDataFrameResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATA_ID_FIELD_NUMBER: builtins.int
    data_id: builtins.str
    def __init__(
        self,
        *,
        data_id: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["data_id", b"data_id"]) -> None: ...

global___AddShareDataFrameResponse = AddShareDataFrameResponse
