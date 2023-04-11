from typing import Iterable, List, Optional

from quickmpc import JobStatus


def make_statuses_detailed(statuses: Optional[Iterable[JobStatus.ValueType]]) \
        -> Optional[List[str]]:
    if statuses is None:
        return None

    return [
        f"{JobStatus.Name(s)}({s})"
        for s in statuses
    ]
