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


def center(window):
    w = window.winfo_screenwidth()
    h = window.winfo_screenheight()
    size = tuple(int(_) for _ in window.geometry().split('+')[0].split('x'))
    x = w / 2 - size[0] / 2
    y = h / 2 - size[1] / 2 - 10
    window.geometry("%dx%d+%d+%d" % (size + (x, y)))


class App(object):
    def __init__(self, *scenes):
        self._root = tkinter.Tk()
        self._root.wm_title(TITLE_STRING)
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

        self._scenes = scenes
        self._scene_index = 0
        if len(scenes) > 0:
            scene_width = scenes[0].width
            scene_height = scenes[0].height
            w = self._root.winfo_screenwidth()
            h = self._root.winfo_screenheight()
            x = w / 2 - scene_width / 2
            y = h / 2 - scene_height / 2
            self._root.geometry('%dx%d+%d+%d' % (scenes[0].width, scenes[0].height, x, y))

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
        self._canvas.after(0, self._paint)

    def _paint(self):
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

        self._root.title(TITLE_STRING + "- %s" % scene.filename)

        if self._image_on_canvas is None:
            self._image_on_canvas = self._canvas.create_image(0, 0, anchor=tkinter.NW, image=image)
        else:
            self._canvas.itemconfig(self._image_on_canvas, image=image)

        scene.draw(painter)

        self._canvas.img = image

    def run(self):
        self._paint()
        tkinter.mainloop()
