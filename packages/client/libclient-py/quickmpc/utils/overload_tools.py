"""
listの次元によるオーバロードが可能となるような自作dispatch
"""

from functools import update_wrapper


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
    if not isinstance(lst, list):
        return 0
    if not lst:
        return 1
    return 1 + d(lst[0])


def find_element_type(lst) -> type:
    if not isinstance(lst, list):
        return lst.__class__
    if not lst:
        return None.__class__
    return find_element_type(lst[0])


def _get_dim_class(lst: list):
    dim: int = d(lst)
    if dim == 1:
        if len(lst) and isinstance(lst[0], dict):
            return DictList()
        return Dim1()
    elif dim == 2:
        if len(lst) and len(lst[0]) and isinstance(lst[0][0], dict):
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
        registry: dict = {}
        default_function = func

        def wrapper(*args, **kw):
            arg = args[0] if is_static_method else args[1]
            type = _convert_list_type(registry, arg)
            elem_type = find_element_type(arg)
            key = (type.__class__, elem_type)
            func = default_function
            if type.__class__ in registry:
                func = registry[type.__class__]
            if key in registry:
                func = registry[key]
            if is_static_method:
                return func.__func__(*args, **kw)
            else:
                return func(*args, **kw)

        def register(types, func=None):
            if func is None:
                return lambda f: register(types, f)
            registry[types] = func
            return func

        wrapper.register = register
        update_wrapper(wrapper, func)
        return wrapper
    return _dimdispatch
