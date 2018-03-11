# -*- coding:utf-8 -*-

import app
from scene import Scene
import os

TEST_ALL = False
GENERATE_BIN = True
if TEST_ALL:
    yamls = [
        'Scene/tv.yaml',
        'Scene/ico.yaml',
        'Scene/monitor.yaml',
        'Scene/symbol.yaml',
        'Scene/button.yaml',
        'Scene/color.yaml',
        'Scene/form.yaml',
        'Scene/label.yaml',
        'Scene/animation.yaml',
        'scene/hello.yaml',
        'scene/line.yaml',
        'scene/menu.yaml',
        'scene/rect.yaml',
        'scene/progressbar.yaml',
        'scene/move.yaml']
else:
    yamls = ['Scene/tv.yaml']

if __name__ == '__main__':
    scenes = []
    for yaml in yamls:
        scene = Scene(yaml)
        scenes.append(scene)
        if GENERATE_BIN:
            scene.generate_binary(target_folder='../Debug/hello.generated/', ram_base_addr=0xF0000000)

    application = app.App(*scenes)
    application.run()
