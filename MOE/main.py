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
    #yamls = ['Scene/monitor.yaml']
    #yamls = ['Scene/symbol.yaml']
    yamls = ['Scene/menu.yaml']
    #yamls = ['Scene/screensaver.yaml']


TARGET_FOLDER = '../DEBUG/'

if __name__ == '__main__':
    scenes = []
    for yaml in yamls:
        scene = Scene(yaml)
        scenes.append(scene)
        if GENERATE_BIN:
            if TEST_ALL:
                target_filename = TARGET_FOLDER + os.path.splitext(os.path.basename(yaml))[0] + '.osd'
            else:
                target_filename = TARGET_FOLDER + 'test.osd'
            scene.generate_binary(target_filename=target_filename, taget_address=0)

    application = app.App(*scenes)
    application.run()
