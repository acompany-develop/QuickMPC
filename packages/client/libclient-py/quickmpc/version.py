try:
    from quickmpc._version import __version__  # type: ignore
except ImportError:
    try:
        from setuptools_scm import get_version  # type: ignore
        __version__ = get_version(root='../../../', relative_to=__file__)
    except (ImportError, LookupError):
        __version__ = "UNKNOWN"
