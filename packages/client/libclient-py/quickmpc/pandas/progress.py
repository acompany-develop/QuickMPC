import collections
from dataclasses import dataclass, field
from typing import Optional, OrderedDict, Tuple

import tqdm

from ..proto.common_types.common_types_pb2 import JobStatus
from ..request.response import GetComputationStatusResponse


@dataclass
class Progress:
    pbars: OrderedDict[Tuple[int, int],
                       Tuple[Optional[tqdm.tqdm], float]] \
        = field(default_factory=collections.OrderedDict)

    # TODO: statusとprogreassを受け取るようにする(疎にするため)
    def update(self, res: GetComputationStatusResponse):
        if res.job_statuses is not None:
            for party_id, status in enumerate(res.job_statuses):
                key = (party_id, -1)

                if key not in self.pbars:
                    self.pbars[(party_id, -1)] = (tqdm.tqdm(
                        desc=f"[{party_id}] status",
                        total=len(JobStatus.items()) - 1), 0)

                pbar, prev = self.pbars[key]
                if pbar is None:
                    continue

                pbar.update(status - prev)
                pbar.set_postfix(
                    status=JobStatus.Name(status)
                )

                if status == JobStatus.Value('COMPLETED'):
                    pbar.close()
                    pbar = None

                self.pbars[key] = (pbar, status)

        progresses = res.progresses
        if progresses is not None:
            for party_id, progress in enumerate(progresses):
                if progress is None:
                    continue

                for procedure in progress.progresses:
                    key = (party_id, procedure.id)

                    if key not in self.pbars:
                        self.pbars[key] = (tqdm.tqdm(
                            desc=f"[{party_id}] {procedure.description}",
                            total=100), 0)

                    pbar, prev = self.pbars[key]
                    if pbar is None:
                        continue

                    pbar.update(procedure.progress - prev)
                    pbar.set_postfix(details=procedure.details)

                    if procedure.completed:
                        pbar.close()
                        pbar = None

                    self.pbars[key] = (pbar, procedure.progress)

        if all([s == JobStatus.COMPLETED for s in res.job_statuses]):
            for key in self.pbars:
                pbar, prev = self.pbars[key]
                if pbar is None:
                    continue
                pbar.update(100 - prev)
                pbar.clear()
                pbar.close()
