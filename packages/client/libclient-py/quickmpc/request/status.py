from enum import Enum


class Status(Enum):
    OK: int = 200
    NotFound: int = 404
    BadGateway: int = 502
