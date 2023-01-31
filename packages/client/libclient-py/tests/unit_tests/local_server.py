""" テスト用のgrpc server """
from concurrent import futures

import grpc
from google.protobuf import any_pb2
from google.rpc import code_pb2, status_pb2
from grpc_status import rpc_status

from quickmpc.proto import libc_to_manage_pb2, libc_to_manage_pb2_grpc
from quickmpc.proto.common_types import common_types_pb2
from quickmpc.proto.common_types.common_types_pb2 import (JobErrorInfo,
                                                          Stacktrace)


class LibToManageServiceServicer(libc_to_manage_pb2_grpc.LibcToManageServicer):

    def __init__(self): ...

    def SendShares(self, request, context):
        res = libc_to_manage_pb2.SendSharesResponse(
            message="ok",
            is_ok=True
        )
        return res

    def DeleteShares(self, request, context):
        res = libc_to_manage_pb2.DeleteSharesResponse(
            message="ok",
            is_ok=True
        )
        return res

    def ExecuteComputation(self, request, context):
        if len(request.arg.src) > 0 \
                and request.arg.src[0] == 1000000000:
            # QMPCJobError
            # CC で Job 実行時にエラーが発生していた場合を再現
            detail = any_pb2.Any()
            detail.Pack(
                JobErrorInfo(
                    what="QMPCJobError",
                )
            )
            rich_status = status_pb2.Status(
                code=code_pb2.INVALID_ARGUMENT,
                details=[detail]
            )
            context.abort_with_status(rpc_status.to_status(rich_status))
            return libc_to_manage_pb2.ExecuteComputationResponse()

        if len(request.table.dataIds) > 0 \
                and request.table.dataIds[0] == "UnregisteredDataId":
            # QMPCServerError
            # MC で Internal Server Error が発生している場合を再現
            context.set_code(grpc.StatusCode.UNKNOWN)
            return libc_to_manage_pb2.ExecuteComputationResponse()

        res = libc_to_manage_pb2.ExecuteComputationResponse(
            message="ok",
            is_ok=True,
            job_uuid="jobjobjob"
        )
        return res

    def GetComputationResult(self, request, context):
        yield libc_to_manage_pb2.GetComputationResultResponse(
            message="ok",
            is_ok=True,
            result=["1"],
            status=common_types_pb2.COMPLETED,
            piece_id=1,
            column_number=2,
            is_dim1=True,
        )
        yield libc_to_manage_pb2.GetComputationResultResponse(
            message="ok",
            is_ok=True,
            result=["2"],
            status=common_types_pb2.COMPLETED,
            piece_id=2,
            column_number=2,
            is_dim1=True,
        )

    def GetDataList(self, request, context):
        res = libc_to_manage_pb2.GetDataListResponse(
            is_ok=True,
            result="[]"
        )
        return res

    def GetJoinTable(self, request, context):
        res = libc_to_manage_pb2.GetJoinTableResponse(
            is_ok=True,
            result="['1']",
            schema=["s"]
        )
        return res

    def GetJobErrorInfo(self, request, context):
        res = libc_to_manage_pb2.GetJobErrorInfoResponse(
            is_ok=True,
            job_error_info=JobErrorInfo(
                what="QMPCJobError",
                stacktrace=Stacktrace()
            )
        )
        return res


def serve(num: int):
    """ server setting """
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    libc_to_manage_pb2_grpc.add_LibcToManageServicer_to_server(
        LibToManageServiceServicer(), server)
    server.add_insecure_port('localhost:900{}'.format(num))

    return server
