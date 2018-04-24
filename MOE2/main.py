# -*- coding:utf-8 -*-

import app
from scene import Scene
import os

TEST_ALL = False
GENERATE_BIN = True
if TEST_ALL:
    yamls = [
        'scene/animation.yaml',
        'scene/bitmap.yaml',
        'scene/button.yaml',
        'scene/font.yaml',
        'scene/hello.yaml',
        'scene/ico.yaml',
        'scene/inputbox.yaml',
        'scene/label.yaml',
        'scene/line.yaml',
        'scene/menu.yaml',
        'scene/monitor.yaml',
        'scene/neg_block.yaml',
        'scene/progressbar.yaml',
        'scene/rect.yaml',
        'scene/screensaver.yaml',
        'scene/text.yaml',
        'scene/tv.yaml',
        'atv/no_signal.yaml',
        'atv/system_settings.yaml']
else:
    #yamls = ['scene/hello.yaml']
    #yamls = ['atv/no_signal.yaml']
    #yamls = ['atv/system_settings.yaml']
    #yamls = ['scene/monitor.yaml']
    yamls = ['scene/hello.yaml']

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
