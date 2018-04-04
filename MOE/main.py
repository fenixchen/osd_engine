# -*- coding:utf-8 -*-

import app
from scene import Scene
import os

TEST_ALL = True
GENERATE_BIN = True
if TEST_ALL:
    yamls = [
        'Scene/tv.yaml',
        'Scene/ico.yaml',
        'Scene/bitmap.yaml',
        'Scene/monitor.yaml',
        'Scene/symbol.yaml',
        'Scene/inputbox.yaml',
        'Scene/color.yaml',
        'Scene/form.yaml',
        'Scene/label.yaml',
        'Scene/label2.yaml',
        'Scene/animation.yaml',
        'scene/hello.yaml',
        'scene/line.yaml',
        'scene/menu.yaml',
        'scene/rect.yaml',
        'scene/progressbar.yaml',
        'scene/move.yaml',
        'scene/screensaver.yaml',
         'scene/button.yaml']
else:
    yamls = ['scene/animation.yaml']
    #yamls = ['atv/system_settings.yaml']

TARGET_FOLDER = '../scenes/'

#TARGET_FOLDER = '../atv/'

if __name__ == '__main__':
    scenes = []
    for yaml in yamls:
        scene = Scene(yaml)
        scenes.append(scene)
        if GENERATE_BIN:
            base_path = TARGET_FOLDER + os.path.splitext(os.path.basename(yaml))[0]
            scene.generate_binary(base_path + '.osd', base_path + '.h', taget_address=0)

    application = app.App(*scenes)
    application.run()
