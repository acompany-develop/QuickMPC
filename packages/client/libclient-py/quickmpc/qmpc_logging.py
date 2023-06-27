import logging

_logger = logging.getLogger("quickmpc")
_logger.addHandler(logging.NullHandler())


def get_logger():
    """quickmpc-libclient-pyのloggerを取得する

    Parameters
    ----------

    Returns
    ----------
    logging.Logger
        quickmpc-libclient-pyのlogger
    """
    return _logger
