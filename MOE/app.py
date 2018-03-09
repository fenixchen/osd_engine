# -*- coding: UTF-8 -*-

import tkinter
import tkinter.filedialog

import scene
from log import Log

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
        self._root = tkinter.Tk()
        self._root.wm_title(TITLE_STRING)
        self._root.geometry('640x480')
        self._menubar = tkinter.Menu(self._root)
        self._fmenu1 = tkinter.Menu(self._root, tearoff=0)
        self._fmenu1.add_command(label='Open', command=self.open)
        self._fmenu1.add_command(label='Export binary...', command=self.export_binary)
        self._fmenu1.add_separator()
        self._fmenu1.add_command(label='Exit', command=self.quit)
        self._menubar.add_cascade(label='File', menu=self._fmenu1)
        self._root.config(menu=self._menubar)

        self._canvas = None
        self._image_on_canvas = None

        self._frame_index = 0
        self._scenes = scenes
        self._scene_index = 0

    def quit(self):
        self._root.quit()
        self._root.destroy()
        exit()

    def open(self):
        yaml_file = tkinter.filedialog.askopenfilename(
            title='Select OSD YAML FIle',
            filetypes=(("YAML files", "*.yaml"), ("all files", "*.*")))
        if len(yaml_file) == 0:
            return

        self._frame_index = 0
        s = scene.Scene(yaml_file)
        self._scenes = [s]
        self._root.geometry('%dx%d' % (s.width, s.height))
        self._scene_index = 0
        self._paint()

    def export_binary(self):
        if self._scene_index >= len(self._scenes):
            return

        bin_folder = tkinter.filedialog.askdirectory(title='Select binary folder')

        if len(bin_folder) == 0:
            return

        s = self._scenes[self._scene_index]
        s.generate_binary(target_folder=bin_folder, ram_base_addr=0xF000000)

    def mouse_click(self, event):
        if event.num == 1:
            self._scene_index = (self._scene_index + 1) % len(self._scenes)
        elif event.num == 3:
            self._scene_index = (self._scene_index - 1 + len(self._scenes)) % len(self._scenes)
        self._frame_index = 0
        self._canvas.after(0, self._paint)

    def _paint(self):
        if self._scene_index >= len(self._scenes):
            return

        scene = self._scenes[self._scene_index]
        if self._canvas is None:
            self._canvas = tkinter.Canvas(self._root,
                                          width=scene.width,
                                          height=scene.height,
                                          bg="#000000")

            self._canvas.bind('<Button-1>', self.mouse_click)
            self._canvas.bind('<Button-3>', self.mouse_click)
            self._canvas.pack()
        image = tkinter.PhotoImage(width=scene.width, height=scene.height)

        painter = Painter(image)

        self._root.title(TITLE_STRING + "[%d/%d] - %s" % (
            self._frame_index + 1,
            scene.frames,
            scene.filename))

        if self._image_on_canvas is None:
            self._image_on_canvas = self._canvas.create_image(0, 0, anchor=tkinter.NW, image=image)
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
        tkinter.mainloop()
