# -*- coding:utf-8 -*-

import abc


class OSDObject(metaclass=abc.ABCMeta):
    def __init__(self, scene, id):
        self._object_index = None
        self._scene = scene
        self._id = id

    @property
    def id(self):
        return self._id

    @property
    def scene(self):
        return self._scene

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
