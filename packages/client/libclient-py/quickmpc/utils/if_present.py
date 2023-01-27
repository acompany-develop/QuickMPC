from typing import Any, Callable, Optional


def if_present(optional: Optional[Any],
               func: Callable[[Any], Any],
               *arg: Any
               ) -> Optional[Any]:
    if optional is None:
        return None
    return func(optional, *arg)
