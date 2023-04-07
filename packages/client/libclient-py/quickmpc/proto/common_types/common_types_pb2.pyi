"""
@generated by mypy-protobuf.  Do not edit manually!
isort:skip_file
"""
import builtins
import collections.abc
import google.protobuf.descriptor
import google.protobuf.internal.containers
import google.protobuf.internal.enum_type_wrapper
import google.protobuf.message
import sys
import typing

if sys.version_info >= (3, 10):
    import typing as typing_extensions
else:
    import typing_extensions

DESCRIPTOR: google.protobuf.descriptor.FileDescriptor

class _JobStatus:
    ValueType = typing.NewType("ValueType", builtins.int)
    V: typing_extensions.TypeAlias = ValueType

class _JobStatusEnumTypeWrapper(google.protobuf.internal.enum_type_wrapper._EnumTypeWrapper[_JobStatus.ValueType], builtins.type):  # noqa: F821
    DESCRIPTOR: google.protobuf.descriptor.EnumDescriptor
    UNKNOWN: _JobStatus.ValueType  # 0
    ERROR: _JobStatus.ValueType  # 1
    RECEIVED: _JobStatus.ValueType  # 2
    PRE_JOB: _JobStatus.ValueType  # 3
    READ_DB: _JobStatus.ValueType  # 4
    COMPUTE: _JobStatus.ValueType  # 5
    COMPLETED: _JobStatus.ValueType  # 6

class JobStatus(_JobStatus, metaclass=_JobStatusEnumTypeWrapper): ...

UNKNOWN: JobStatus.ValueType  # 0
ERROR: JobStatus.ValueType  # 1
RECEIVED: JobStatus.ValueType  # 2
PRE_JOB: JobStatus.ValueType  # 3
READ_DB: JobStatus.ValueType  # 4
COMPUTE: JobStatus.ValueType  # 5
COMPLETED: JobStatus.ValueType  # 6
global___JobStatus = JobStatus

class _ComputationMethod:
    ValueType = typing.NewType("ValueType", builtins.int)
    V: typing_extensions.TypeAlias = ValueType

class _ComputationMethodEnumTypeWrapper(google.protobuf.internal.enum_type_wrapper._EnumTypeWrapper[_ComputationMethod.ValueType], builtins.type):  # noqa: F821
    DESCRIPTOR: google.protobuf.descriptor.EnumDescriptor
    COMPUTATION_METHOD_UNSPECIFIED: _ComputationMethod.ValueType  # 0
    COMPUTATION_METHOD_MEAN: _ComputationMethod.ValueType  # 1
    COMPUTATION_METHOD_VARIANCE: _ComputationMethod.ValueType  # 2
    COMPUTATION_METHOD_SUM: _ComputationMethod.ValueType  # 3
    COMPUTATION_METHOD_CORREL: _ComputationMethod.ValueType  # 4
    COMPUTATION_METHOD_MESH_CODE: _ComputationMethod.ValueType  # 5
    COMPUTATION_METHOD_JOIN_TABLE: _ComputationMethod.ValueType  # 6

class ComputationMethod(_ComputationMethod, metaclass=_ComputationMethodEnumTypeWrapper): ...

COMPUTATION_METHOD_UNSPECIFIED: ComputationMethod.ValueType  # 0
COMPUTATION_METHOD_MEAN: ComputationMethod.ValueType  # 1
COMPUTATION_METHOD_VARIANCE: ComputationMethod.ValueType  # 2
COMPUTATION_METHOD_SUM: ComputationMethod.ValueType  # 3
COMPUTATION_METHOD_CORREL: ComputationMethod.ValueType  # 4
COMPUTATION_METHOD_MESH_CODE: ComputationMethod.ValueType  # 5
COMPUTATION_METHOD_JOIN_TABLE: ComputationMethod.ValueType  # 6
global___ComputationMethod = ComputationMethod

class _ShareValueTypeEnum:
    ValueType = typing.NewType("ValueType", builtins.int)
    V: typing_extensions.TypeAlias = ValueType

class _ShareValueTypeEnumEnumTypeWrapper(google.protobuf.internal.enum_type_wrapper._EnumTypeWrapper[_ShareValueTypeEnum.ValueType], builtins.type):  # noqa: F821
    DESCRIPTOR: google.protobuf.descriptor.EnumDescriptor
    SHARE_VALUE_TYPE_UNSPECIFIED: _ShareValueTypeEnum.ValueType  # 0
    SHARE_VALUE_TYPE_FIXED_POINT: _ShareValueTypeEnum.ValueType  # 1
    SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION: _ShareValueTypeEnum.ValueType  # 2

class ShareValueTypeEnum(_ShareValueTypeEnum, metaclass=_ShareValueTypeEnumEnumTypeWrapper): ...

SHARE_VALUE_TYPE_UNSPECIFIED: ShareValueTypeEnum.ValueType  # 0
SHARE_VALUE_TYPE_FIXED_POINT: ShareValueTypeEnum.ValueType  # 1
SHARE_VALUE_TYPE_UTF_8_INTEGER_REPRESENTATION: ShareValueTypeEnum.ValueType  # 2
global___ShareValueTypeEnum = ShareValueTypeEnum

class ProcedureProgress(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    ID_FIELD_NUMBER: builtins.int
    DESCRIPTION_FIELD_NUMBER: builtins.int
    PROGRESS_FIELD_NUMBER: builtins.int
    COMPLETED_FIELD_NUMBER: builtins.int
    DETAILS_FIELD_NUMBER: builtins.int
    id: builtins.int
    """unique id in ProgressManager context"""
    description: builtins.str
    """describe specific process in job"""
    progress: builtins.float
    """describe progress by [0.0, 100.0]"""
    completed: builtins.bool
    """describe that procedure is completed or not"""
    details: builtins.str
    """describe progress in details"""
    def __init__(
        self,
        *,
        id: builtins.int = ...,
        description: builtins.str = ...,
        progress: builtins.float = ...,
        completed: builtins.bool = ...,
        details: builtins.str | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_details", b"_details", "details", b"details"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_details", b"_details", "completed", b"completed", "description", b"description", "details", b"details", "id", b"id", "progress", b"progress"]) -> None: ...
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_details", b"_details"]) -> typing_extensions.Literal["details"] | None: ...

global___ProcedureProgress = ProcedureProgress

class JobProgress(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    JOB_UUID_FIELD_NUMBER: builtins.int
    STATUS_FIELD_NUMBER: builtins.int
    PROGRESSES_FIELD_NUMBER: builtins.int
    job_uuid: builtins.str
    status: global___JobStatus.ValueType
    @property
    def progresses(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[global___ProcedureProgress]: ...
    def __init__(
        self,
        *,
        job_uuid: builtins.str = ...,
        status: global___JobStatus.ValueType = ...,
        progresses: collections.abc.Iterable[global___ProcedureProgress] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["job_uuid", b"job_uuid", "progresses", b"progresses", "status", b"status"]) -> None: ...

global___JobProgress = JobProgress

class Stacktrace(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    class Frame(google.protobuf.message.Message):
        DESCRIPTOR: google.protobuf.descriptor.Descriptor

        SOURCE_LOCATION_FIELD_NUMBER: builtins.int
        SOURCE_LINE_FIELD_NUMBER: builtins.int
        FUNCTION_NAME_FIELD_NUMBER: builtins.int
        source_location: builtins.str
        source_line: builtins.int
        function_name: builtins.str
        def __init__(
            self,
            *,
            source_location: builtins.str = ...,
            source_line: builtins.int = ...,
            function_name: builtins.str = ...,
        ) -> None: ...
        def ClearField(self, field_name: typing_extensions.Literal["function_name", b"function_name", "source_line", b"source_line", "source_location", b"source_location"]) -> None: ...

    FRAMES_FIELD_NUMBER: builtins.int
    @property
    def frames(self) -> google.protobuf.internal.containers.RepeatedCompositeFieldContainer[global___Stacktrace.Frame]: ...
    def __init__(
        self,
        *,
        frames: collections.abc.Iterable[global___Stacktrace.Frame] | None = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["frames", b"frames"]) -> None: ...

global___Stacktrace = Stacktrace

class JobErrorInfo(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    WHAT_FIELD_NUMBER: builtins.int
    ADDITIONAL_INFO_FIELD_NUMBER: builtins.int
    STACKTRACE_FIELD_NUMBER: builtins.int
    what: builtins.str
    additional_info: builtins.str
    @property
    def stacktrace(self) -> global___Stacktrace: ...
    def __init__(
        self,
        *,
        what: builtins.str = ...,
        additional_info: builtins.str | None = ...,
        stacktrace: global___Stacktrace | None = ...,
    ) -> None: ...
    def HasField(self, field_name: typing_extensions.Literal["_additional_info", b"_additional_info", "_stacktrace", b"_stacktrace", "additional_info", b"additional_info", "stacktrace", b"stacktrace"]) -> builtins.bool: ...
    def ClearField(self, field_name: typing_extensions.Literal["_additional_info", b"_additional_info", "_stacktrace", b"_stacktrace", "additional_info", b"additional_info", "stacktrace", b"stacktrace", "what", b"what"]) -> None: ...
    @typing.overload
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_additional_info", b"_additional_info"]) -> typing_extensions.Literal["additional_info"] | None: ...
    @typing.overload
    def WhichOneof(self, oneof_group: typing_extensions.Literal["_stacktrace", b"_stacktrace"]) -> typing_extensions.Literal["stacktrace"] | None: ...

global___JobErrorInfo = JobErrorInfo

class Schema(google.protobuf.message.Message):
    DESCRIPTOR: google.protobuf.descriptor.Descriptor

    NAME_FIELD_NUMBER: builtins.int
    TYPE_FIELD_NUMBER: builtins.int
    name: builtins.str
    type: global___ShareValueTypeEnum.ValueType
    def __init__(
        self,
        *,
        name: builtins.str = ...,
        type: global___ShareValueTypeEnum.ValueType = ...,
    ) -> None: ...
    def ClearField(self, field_name: typing_extensions.Literal["name", b"name", "type", b"type"]) -> None: ...

global___Schema = Schema
