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
import typing

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

class SendSharesResponse(google.protobuf.message.Message):
    """*
    the message of SendSharesResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    MESSAGE_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    message: builtins.str
    is_ok: builtins.bool
    def __init__(
        self,
        *,
        message: builtins.str = ...,
        is_ok: builtins.bool = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["is_ok", b"is_ok", "message", b"message"]) -> None: ...

global___SendSharesResponse = SendSharesResponse

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

class DeleteSharesResponse(google.protobuf.message.Message):
    """*
    the message of DeleteSharesResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    MESSAGE_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    message: builtins.str
    is_ok: builtins.bool
    def __init__(
        self,
        *,
        message: builtins.str = ...,
        is_ok: builtins.bool = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["is_ok", b"is_ok", "message", b"message"]) -> None: ...

global___DeleteSharesResponse = DeleteSharesResponse

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

    MESSAGE_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    SCHEMA_FIELD_NUMBER: builtins.int
    message: builtins.str
    is_ok: builtins.bool
    @property
    def schema(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[common_types.common_types_pb2.Schema]: ...
    def __init__(
        self,
        *,
        message: builtins.str = ...,
        is_ok: builtins.bool = ...,
        schema: collections.abc.Iterable[common_types.common_types_pb2.Schema] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["is_ok", b"is_ok", "message", b"message", "schema", b"schema"]) -> None: ...

global___GetSchemaResponse = GetSchemaResponse

class JoinOrder(google.protobuf.message.Message):
    """*
    the message of ExecuteComputationRequest
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    DATAIDS_FIELD_NUMBER: builtins.int
    JOIN_FIELD_NUMBER: builtins.int
    INDEX_FIELD_NUMBER: builtins.int
    @property
    def dataIds(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.str]: ...
    @property
    def join(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.int]: ...
    @property
    def index(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.int]: ...
    def __init__(
        self,
        *,
        dataIds: collections.abc.Iterable[builtins.str] | None = ...,
        join: collections.abc.Iterable[builtins.int] | None = ...,
        index: collections.abc.Iterable[builtins.int] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["dataIds", b"dataIds", "index", b"index", "join", b"join"]) -> None: ...

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

    MESSAGE_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    JOB_UUID_FIELD_NUMBER: builtins.int
    message: builtins.str
    is_ok: builtins.bool
    job_uuid: builtins.str
    def __init__(
        self,
        *,
        message: builtins.str = ...,
        is_ok: builtins.bool = ...,
        job_uuid: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["is_ok", b"is_ok", "job_uuid", b"job_uuid", "message", b"message"]) -> None: ...

global___ExecuteComputationResponse = ExecuteComputationResponse

class GetComputationResultRequest(google.protobuf.message.Message):
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

global___GetComputationResultRequest = GetComputationResultRequest

class GetComputationResultResponse(google.protobuf.message.Message):
    """*
    the message of GetComputationResultResponse
    """

    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    MESSAGE_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    RESULT_FIELD_NUMBER: builtins.int
    COLUMN_NUMBER_FIELD_NUMBER: builtins.int
    STATUS_FIELD_NUMBER: builtins.int
    PIECE_ID_FIELD_NUMBER: builtins.int
    PROGRESS_FIELD_NUMBER: builtins.int
    IS_DIM1_FIELD_NUMBER: builtins.int
    IS_DIM2_FIELD_NUMBER: builtins.int
    IS_SCHEMA_FIELD_NUMBER: builtins.int
    message: builtins.str
    is_ok: builtins.bool
    @property
    def result(self) -> google.protobuf.internal.containers.RepeatedScalarFieldContainer[builtins.str]: ...
    column_number: builtins.int
    status: common_types.common_types_pb2.JobStatus.ValueType
    piece_id: builtins.int
    @property
    def progress(self) -> common_types.common_types_pb2.JobProgress: ...
    is_dim1: builtins.bool
    is_dim2: builtins.bool
    is_schema: builtins.bool
    def __init__(
        self,
        *,
        message: builtins.str = ...,
        is_ok: builtins.bool = ...,
        result: collections.abc.Iterable[builtins.str] | None = ...,
        column_number: builtins.int = ...,
        status: common_types.common_types_pb2.JobStatus.ValueType = ...,
        piece_id: builtins.int = ...,
        progress: common_types.common_types_pb2.JobProgress | None = ...,
        is_dim1: builtins.bool = ...,
        is_dim2: builtins.bool = ...,
        is_schema: builtins.bool = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_progress", b"_progress", "is_dim1", b"is_dim1", "is_dim2", b"is_dim2", "is_schema", b"is_schema", "progress", b"progress", "result_type", b"result_type"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_progress", b"_progress", "column_number", b"column_number", "is_dim1", b"is_dim1", "is_dim2", b"is_dim2", "is_ok", b"is_ok", "is_schema", b"is_schema", "message", b"message", "piece_id", b"piece_id", "progress", b"progress", "result", b"result", "result_type", b"result_type", "status", b"status"]) -> None: ...
    @typing.overload
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_progress", b"_progress"]) -> typing_extensions.Literal["progress"] | None: ...
    @typing.overload
    def WhichOneof(self, oneof_group: typing_extensions.Literal["result_type", b"result_type"]) -> typing_extensions.Literal["is_dim1", "is_dim2", "is_schema"] | None: ...

global___GetComputationResultResponse = GetComputationResultResponse

class GetDataListRequest(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    TOKEN_FIELD_NUMBER: builtins.int
    token: builtins.str
    def __init__(
        self,
        *,
        token: builtins.str = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["token", b"token"]) -> None: ...

global___GetDataListRequest = GetDataListRequest

class GetDataListResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    RESULT_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    result: builtins.str
    is_ok: builtins.bool
    def __init__(
        self,
        *,
        result: builtins.str = ...,
        is_ok: builtins.bool = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["is_ok", b"is_ok", "result", b"result"]) -> None: ...

global___GetDataListResponse = GetDataListResponse

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

    IS_OK_FIELD_NUMBER: builtins.int
    ELAPSED_TIME_FIELD_NUMBER: builtins.int
    is_ok: builtins.bool
    elapsed_time: builtins.float
    def __init__(
        self,
        *,
        is_ok: builtins.bool = ...,
        elapsed_time: builtins.float = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["elapsed_time", b"elapsed_time", "is_ok", b"is_ok"]) -> None: ...

global___GetElapsedTimeResponse = GetElapsedTimeResponse

class GetJobErrorInfoRequest(google.protobuf.message.Message):
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

global___GetJobErrorInfoRequest = GetJobErrorInfoRequest

class GetJobErrorInfoResponse(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_ERROR_INFO_FIELD_NUMBER: builtins.int
    IS_OK_FIELD_NUMBER: builtins.int
    @property
    def job_error_info(self) -> common_types.common_types_pb2.JobErrorInfo: ...
    is_ok: builtins.bool
    def __init__(
        self,
        *,
        job_error_info: common_types.common_types_pb2.JobErrorInfo | None = ...,
        is_ok: builtins.bool = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_job_error_info", b"_job_error_info", "job_error_info", b"job_error_info"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_job_error_info", b"_job_error_info", "is_ok", b"is_ok", "job_error_info", b"job_error_info"]) -> None: ...
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_job_error_info", b"_job_error_info"]) -> typing_extensions.Literal["job_error_info"] | None: ...

global___GetJobErrorInfoResponse = GetJobErrorInfoResponse
