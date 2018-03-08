# -*- coding:utf-8 -*-

import abc


class OSDObject(metaclass=abc.ABCMeta):

    def __init__(self):
        self._object_index = None

    @property
    def object_index(self):
        assert self._object_index is not None
        return self._object_index

    @object_index.setter
    def object_index(self, object_index):
        self._object_index = object_index

    @abc.abstractmethod
    def to_binary(self, ram_offset):
        pass
