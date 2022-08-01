# INFO: 設計の参考になりそう https://github.com/ymyzk/wsgi_lineprof

import argparse
import parser
from file_io import read_log_file
from parser import parse_log_file, get_log_info
from datetime import datetime, timedelta
from typing import List, Optional


class Row:
    __slots__ = ('_label', '_command', '_start_time',
                 '_proc_time', '_numbering')

    _label: str
    _command: str
    _start_time: datetime
    _proc_time: timedelta
    _numbering: Optional[int]

    def __init__(self, label: str, command: str, start_time: datetime, proc_time: timedelta, numbering: Optional[int] = None) -> None:
        self._label = label
        self._command = command
        self._start_time = start_time
        self._proc_time = proc_time
        self._numbering = numbering

    def __repr__(self) -> str:
        return f"{self._label[:35]: <40} {self._command[:60]:.<70}  {round(self._proc_time.total_seconds(), 1)}s"


def visualize_parsed_log_file(parsed_log_file: str, order_type: str) -> None:
    times_result: List[Row] = []
    lines = parsed_log_file.split("\n")

    PROC_TIME_INITIALIZE: timedelta = timedelta(0)

    group_command: Row = None
    previous_command: Row = None
    hash_num: str = '0'

    for line in lines:
        flag, time, num, label, command = get_log_info(line)

        if time is None:
            continue

        if flag == "<4>":
            if hash_num != num and previous_command is not None:
                previous_command._proc_time = time - previous_command._start_time
                times_result.append(previous_command)
                previous_command = Row(
                    "", command, time, PROC_TIME_INITIALIZE, None)
        else:
            if previous_command is not None:
                previous_command._proc_time = time - previous_command._start_time
                times_result.append(previous_command)
                previous_command = None

            if flag == "<1>":
                if label == "[group]":
                    group_command = Row(
                        label, command, time, PROC_TIME_INITIALIZE, None)
                elif label == "[endgroup]":
                    group_command._proc_time = time - group_command._start_time
                    times_result.append(group_command)
            else:
                previous_command = Row(
                    label, command, time, PROC_TIME_INITIALIZE, None)

        if num is not None:
            hash_num = num

    if order_type == 'asc':
        times_result.sort(key=lambda x: x._proc_time)
    elif order_type == 'desc':
        times_result.sort(key=lambda x: x._proc_time, reverse=True)

    total_time = timedelta(0)
    for row in times_result:
        print(row)
        total_time += row._proc_time
    print()
    print(
        f"{'[TOTAL TIME]': <111}  {round(total_time.total_seconds(), 1)}s")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("log_file_path")
    parser.add_argument("--order_by", help="asc or desc")
    args = parser.parse_args()
    log_file = read_log_file(args.log_file_path)
    parsed_log_file = parse_log_file(log_file)
    visualize_parsed_log_file(parsed_log_file, args.order_by)
