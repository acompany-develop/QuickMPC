from enum import Enum


class Status(Enum):
    BadGateway: int = 502
    NotFound: int = 404
