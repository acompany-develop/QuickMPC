import re
import datetime

DATE_TIME_REGEX = "[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{7}Z"


def parse_log_file(log_file_content: str) -> str:
    parsed_log_file = re.sub(
        rf"({DATE_TIME_REGEX}\s\[.+\])", "<2>\\1", log_file_content)
    parsed_log_file = re.sub(
        rf"({DATE_TIME_REGEX}\s#[0-9]+ \[.+\])", "<3>\\1", parsed_log_file)
    parsed_log_file = re.sub(
        rf"({DATE_TIME_REGEX})\s##", "<1>\\1 ", parsed_log_file)
    parsed_log_file = re.sub(
        rf"({DATE_TIME_REGEX}\s#[0-9]+ [^\[].*)", "<4>\\1 ", parsed_log_file)
    parsed_log_file = re.sub(
        rf"^[0-9]{4}.+[\n]*", "", parsed_log_file, flags=re.MULTILINE)
    return parsed_log_file


def get_log_info(line: str):
    regex = {
        "<1>": rf"<([0-9]+)>({DATE_TIME_REGEX})\s()(\[.+\])\s*(.*)",
        "<2>": rf"<([0-9]+)>({DATE_TIME_REGEX})\s()(\[.+\])\s*(.*)",
        "<3>": rf"<([0-9]+)>({DATE_TIME_REGEX})\s#([0-9]+)\s(\[.+?\])\s*(.*)",
        "<4>": rf"<([0-9]+)>({DATE_TIME_REGEX})\s#([0-9]+)()\s*(.*)"
    }
    datetime_format = '%Y-%m-%dT%H:%M:%S.%f'
    flag = line[:3]
    if flag in regex.keys():
        r = re.match(regex[flag], line)
        time = datetime.datetime.strptime(r.group(2)[:-2], datetime_format)
        num = r.group(3)
        label = r.group(4)
        command = r.group(5)
        return flag, time, num, label, command
    return flag, None, None, None, None


def is_done(line: str) -> bool:
    regex = rf"<([0-9]+)>({DATE_TIME_REGEX})\s#(.+?) DONE [0-9]+\.[0-9]s"
    r = re.match(regex, line)
    if r is None:
        return False
    return True
