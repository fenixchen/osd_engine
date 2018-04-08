# -*- coding:utf-8 -*-

import app
from scene import Scene
import os

TEST_ALL = False
GENERATE_BIN = True
if TEST_ALL:
    yamls = [
        'scene/tv.yaml',
        'scene/ico.yaml',
        'scene/bitmap.yaml',
        'scene/monitor.yaml',
        'scene/symbol.yaml',
        'scene/inputbox.yaml',
        'scene/color.yaml',
        'scene/form.yaml',
        'scene/label.yaml',
        'scene/label2.yaml',
        'scene/animation.yaml',
        'scene/hello.yaml',
        'scene/line.yaml',
        'scene/menu.yaml',
        'scene/rect.yaml',
        'scene/progressbar.yaml',
        'scene/move.yaml',
        'scene/screensaver.yaml',
        'scene/button.yaml',
        'scene/neg_block.yaml',
        'atv/system_settings.yaml']
else:
    yamls = ['atv/system_settings.yaml']
    # yamls = ['scene/screensaver.yaml']
    #yamls = ['scene/neg_block.yaml']

if __name__ == '__main__':
    scenes = []
    for yaml in yamls:
        scene = Scene(yaml)
        scenes.append(scene)
        if GENERATE_BIN:
            if yaml.startswith('scene'):
                TARGET_FOLDER = '../scenes/'
            else:
                TARGET_FOLDER = '../atv/'
            base_path = TARGET_FOLDER + os.path.splitext(os.path.basename(yaml))[0]
            scene.generate_binary(base_path + '.osd', base_path + '.h', taget_address=0)

    application = app.App(*scenes)
    application.run()
