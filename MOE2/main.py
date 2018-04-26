# -*- coding:utf-8 -*-

import app
from scene import Scene
import os

TEST_ALL = False
GENERATE_BIN = True
if TEST_ALL:
    yamls = ['scene/hello.yaml'
             ]
else:
    yamls = ['scene/system_settings.yaml']
    #yamls = ['scene/hello.yaml']

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
