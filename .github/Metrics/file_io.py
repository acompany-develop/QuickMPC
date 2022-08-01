def read_log_file(log_file_path: str) -> str:
    with open(log_file_path) as fp:
        log_file = fp.read()
    return log_file


def write_file(string: str, path: str) -> bool:
    with open(path, "w") as fp:
        fp.write(string)
    return True
