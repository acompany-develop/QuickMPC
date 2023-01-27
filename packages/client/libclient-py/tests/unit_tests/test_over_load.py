from quickmpc.utils.overload_tools import (DictList, DictList2, Dim1, Dim2,
                                           Dim3, methoddispatch)


def test_overload():
    """methodが正しくoverloadされるかTest"""

    class Class:
        @methoddispatch()
        def method(self, val):
            return 0

        @method.register(int)
        def method1(self, val):
            return 1

        @method.register(float)
        def method2(self, val):
            return 2

        @method.register(str)
        def method3(self, val):
            return 3

        @method.register(list)
        def method4(self, val):
            return 4

    d: Class = Class()
    assert (d.method({}) == 0)
    assert (d.method(0) == 1)
    assert (d.method(0.0) == 2)
    assert (d.method("0") == 3)
    assert (d.method([0]) == 4)


def test_overload_static():
    """stataic methodが正しくoverloadされるかTest"""
    class Class:
        @methoddispatch(is_static_method=True)
        @staticmethod
        def method(val):
            return 0

        @method.register(int)
        @staticmethod
        def method1(val):
            return 1

        @method.register(float)
        @staticmethod
        def method2(val):
            return 2

        @method.register(str)
        @staticmethod
        def method3(val):
            return 3

        @method.register(list)
        @staticmethod
        def method4(val):
            return 4

    assert (Class.method({}) == 0)
    assert (Class.method(0) == 1)
    assert (Class.method(0.0) == 2)
    assert (Class.method("0") == 3)
    assert (Class.method([0]) == 4)


def test_overload_dim():
    """methodが次元数に応じて正しくoverloadされるかTest"""

    class Class:
        @methoddispatch()
        def method(self, val):
            return 0

        @method.register(int)
        def method1(self, val):
            return 1

        @method.register(Dim1)
        def method2(self, val):
            return 2

        @method.register(Dim2)
        def method3(self, val):
            return 3

        @method.register(Dim3)
        def method4(self, val):
            return 4

        @method.register(DictList)
        def method5(self, val):
            return 5

        @method.register(DictList2)
        def method6(self, val):
            return 6

    d: Class = Class()
    assert (d.method({}) == 0)
    assert (d.method(0) == 1)
    assert (d.method([0]) == 2)
    assert (d.method([[0]]) == 3)
    assert (d.method([[[0]]]) == 4)
    assert (d.method([{}]) == 5)
    assert (d.method([[{}]]) == 6)
    assert (d.method([[[[0]]]]) == 0)
