from typing import Any, Callable, Optional


def if_present(optional: Optional[Any],
               func: Callable[[Any], Any],
               *args: Any
               ) -> Optional[Any]:
    """optional変数を評価して関数を実行する

    Parameters
    ----------
    optional: Optional[Any]
        何らかのoptional変数
    func: Callable[[Any], Any]
        実行する関数
    *args: Any
        実行する関数の引数列

    Returns
    -------
    Optional[Any]
        関数の実行結果
    """
    if optional is None:
        return None
    return func(optional, *args)
