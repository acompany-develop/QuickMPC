"""
listの次元によるオーバロードが可能となるような自作dispatch
"""

from functools import singledispatch, update_wrapper


class Dim1:
    ...


class Dim2:
    ...


class Dim3:
    ...


class DictList:
    ...


class DictList2:
    ...


def d(lst):
    if type(lst) != list:
        return 0
    if not lst:
        return 1
    return 1+d(lst[0])


def _get_dim_class(lst: list):
    dim: int = d(lst)
    if dim == 1:
        if len(lst) and type(lst[0]) == dict:
            return DictList()
        return Dim1()
    elif dim == 2:
        if len(lst) and len(lst[0]) and type(lst[0][0]) == dict:
            return DictList2()
        return Dim2()
    elif dim == 3:
        return Dim3()
    return None


def _convert_list_type(registry, type):
    if type.__class__ is not list:
        return type
    for Dim in (Dim1, Dim2, Dim3, DictList):
        if Dim in registry:
            return _get_dim_class(type)
    return type


def methoddispatch(is_static_method: bool = False):
    def _dimdispatch(func):
        dispatcher = singledispatch(func)

        def wrapper(*args, **kw):
            arg = args[0] if is_static_method else args[1]
            type = _convert_list_type(dispatcher.registry, arg)
            if is_static_method:
                return dispatcher.dispatch(
                    type.__class__
                ).__func__(*args, **kw)
            else:
                return dispatcher.dispatch(
                    type.__class__
                )(*args, **kw)
        wrapper.register = dispatcher.register
        update_wrapper(wrapper, func)
        return wrapper
    return _dimdispatch
