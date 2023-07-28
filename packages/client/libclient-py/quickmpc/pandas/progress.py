import collections
from dataclasses import dataclass, field
from typing import Optional, OrderedDict, Tuple

import tqdm

from quickmpc.proto.common_types.common_types_pb2 import JobStatus
from quickmpc.request.response import GetComputationStatusResponse


@dataclass
class Progress:
    """計算の進捗を管理するクラス

    Attributes
    ----------
    pbars: OrderedDict[Tuple[int, int],
                       Tuple[Optional[tqdm.tqdm], float]]
        計算Statusを保持する辞書

    Examples
    --------
    .. code-block:: python3

        progress = Progress()
        while runinng:
            status = get_status()
            progress.update(status)

    output

    .. code-block:: console

        [0] status: 100%|███████████████| 6/6 [00:01<00:00,  5.88it/s, status=COMPLETED]
        [1] status: 100%|███████████████| 6/6 [00:01<00:00,  5.88it/s, status=COMPLETED]
        [2] status: 100%|███████████████| 6/6 [00:01<00:00,  5.88it/s, status=COMPLETED]
        [0] hjoin: core: 100%|█████████| 100.0/100 [00:01<00:00, 97.96it/s, details=1/4]
        [0] hjoin: binary search: 100%|█| 100.0/100 [00:01<00:00, 97.97it/s, details=0/2
        [1] hjoin: core: 100%|█████████| 100.0/100 [00:01<00:00, 97.98it/s, details=1/4]
        [1] hjoin: binary search: 100%|█| 100.0/100 [00:01<00:00, 97.99it/s, details=0/2
        [2] hjoin: core: 100%|█████████| 100.0/100 [00:01<00:00, 98.00it/s, details=1/4]
        [2] hjoin: binary search: 100%|█| 100.0/100 [00:01<00:00, 98.01it/s, details=0/2
    """  # noqa: E501
    pbars: OrderedDict[Tuple[int, int],
                       Tuple[Optional[tqdm.tqdm], float]] \
        = field(default_factory=collections.OrderedDict)

    # TODO: statusとprogreassを受け取るようにする(疎にするため)
    def update(self, res: GetComputationStatusResponse):
        """進捗Statusを更新する

        Parameters
        ----------
        res: GetComputationStatusResponse
            計算Status

        Returns
        -------
        None
        """
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
