# -*- coding: UTF-8 -*-

from tkinter import *

from engine import *

logger = Log.get_logger("app")


class Painter(object):
    def __init__(self, image):
        self._image = image

    def set_image(self, image):
        self._image = image

    def set_pixel(self, x, y, color):
        self._image.put(color, (x, y))


TITLE_STRING = "Monitor OSD Engine Demo"


class App(object):
    def __init__(self, *scenes):
        self._root = Tk()
        self._root.title(TITLE_STRING)
        self._canvas = None
        self._image_on_canvas = None

        self._frame_index = 0
        self._scenes = scenes
        self._scene_index = 0

    def mouse_click(self, event):
        if event.num == 1:
            self._scene_index = (self._scene_index + 1) % len(self._scenes)
        elif event.num == 3:
            self._scene_index = (self._scene_index - 1 + len(self._scenes)) % len(self._scenes)
        self._frame_index = 0
        self._canvas.after(0, self._paint)

    def _paint(self):
        scene = self._scenes[self._scene_index]
        if self._canvas is None:
            self._canvas = Canvas(self._root,
                                  width=scene.width,
                                  height=scene.height,
                                  bg="#000000")

            self._canvas.bind('<Button-1>', self.mouse_click)
            self._canvas.bind('<Button-3>', self.mouse_click)
            self._canvas.pack()
        image = PhotoImage(width=scene.width, height=scene.height)

        painter = Painter(image)

        self._root.title(TITLE_STRING + "[%d/%d] - %s" % (
            self._frame_index + 1,
            scene.frames,
            scene.filename))

        if self._image_on_canvas is None:
            self._image_on_canvas = self._canvas.create_image(0, 0, anchor=NW, image=image)
        else:
            self._canvas.itemconfig(self._image_on_canvas, image=image)

        scene.draw(painter)

        scene.modify()

        self._canvas.img = image
        self._frame_index = self._frame_index + 1

        if self._frame_index < scene.frames:
            self._canvas.after(scene.ticks, self._paint)

    def run(self):
        self._paint()
        mainloop()
