# -*- coding:utf-8 -*-
from bitmap import *
from plot import *
from window import *

logger = Log.get_logger("engine")


class Modifier(OSDObject):

    @abc.abstractmethod
    def execute(self, window):
        raise Exception("must be implemented by child")

    def __init__(self, scene, id, interval, limit, windows, blocks, active):
        self._scene = scene
        self._id = id
        self._interval = interval
        self._windows = []
        self._blocks = []
        self._active = active
        self._elapsed = 0
        self._counter = 0
        self._limit = limit
        if windows is not None:
            for window in windows:
                obj = scene.find_window(window)
                if obj is not None:
                    self._windows.append(obj)
                else:
                    raise Exception('cannot find window <%s>' % window)
        if blocks is not None:
            for block in blocks:
                obj = scene.find_block(block)
                if obj is not None:
                    self._blocks.append(obj)
                else:
                    raise Exception('cannot find block <%s>' % block)

    @property
    def active(self):
        return self._active

    @active.setter
    def active(self, active):
        self._active = active

    @property
    def id(self):
        return self._id

    def run(self):
        if self._limit > 0:
            if self._counter >= self._limit:
                return
        self._elapsed += 1
        if self._elapsed < self._interval:
            return
        self._elapsed = 0
        self._counter += 1
        for window in self._windows:
            self.execute(window)
        for block in self._blocks:
            self.execute(block)

    def __str__(self):
        ret = '%s(id:%s, interval:%d, limit:%d, active:%d' % (
            type(self),
            self._id, self._interval, self._limit, self.active)
        return ret

    def target_desc(self):
        ret = ''
        for window in self._windows:
            ret += '\twindow: <%s>\n' % window.id
        for block in self._blocks:
            ret += '\tblock: <%s>\n' % block.full_id
        return ret


class Move(Modifier):
    def __init__(self, scene, id,
                 x_delta=0, y_delta=0, w_delta=0, h_delta=0,
                 windows=None, blocks=None, active=True, interval=1, limit=0):

        super().__init__(scene, id, interval, limit, windows, blocks, active)

        self._x_delta, self._y_delta = x_delta, y_delta
        self._w_delta, self._h_delta = w_delta, h_delta

    def execute(self, target):
        if isinstance(target, Window):
            target.x += self._x_delta
            target.y += self._y_delta
            target.width += self._w_delta
            target.height += self._h_delta
        elif isinstance(target, Block):
            target.x += self._x_delta
            target.y += self._y_delta
        else:
            raise Exception("Unhandled type <%s>" % type(target))

    def __str__(self):
        ret = Modifier.__str__(self)
        ret += ", x_delta:%d, y_delta:%d, w_delta:%d, h_delta:%d)\n" % (
            self._x_delta, self._y_delta, self._w_delta, self._h_delta)
        ret += self.target_desc()
        return ret

    def to_binary(self, ram_offset):
        raise Exception('not implemented')


class Flip(Modifier):
    def __init__(self, scene, id,
                 loop=True,
                 windows=None, blocks=None, active=True, interval=1, limit=0):
        super().__init__(scene, id, interval, limit, windows, blocks, active)
        self._loop = loop

    def execute(self, target):
        if isinstance(target, Block) and isinstance(target.ingredient, Bitmap):
            bitmap = target.ingredient
            bitmap.flip(self._loop)
        else:
            raise Exception("Unhandled type <%s>" % type(target))

    def __str__(self):
        ret = Modifier.__str__(self)
        ret += ", loop:%d)\n" % (self._loop)
        ret += self.target_desc()
        return ret

    def to_binary(self, ram_offset):
        raise Exception('not implemented')


"""
class Rotate(Modifier):
    def __init__(self, name, step_angle=15):
        super().__init__(name)
        self._step_angle = step_angle

    def execute(self, target):
        if isinstance(target, Block):
            if isinstance(target.ingredient(), Line):
                line = target.ingredient()
                line.rotate(self._step_angle)
        else:
            raise Exception("Unhandled type %s" % target.__class__.__name__)
"""