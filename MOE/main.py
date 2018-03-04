# -*- coding:utf-8 -*-

import app
from scene import Scene

GEN_BINARY = True

if __name__ == '__main__':
    if not GEN_BINARY:
        app = app.App(
            Scene('scene/line.yaml'),
            Scene('scene/rect.yaml'),
            Scene('scene/progressbar.yaml'),
            Scene('scene/menu.yaml'),
            Scene('scene/button.yaml'),
            Scene('scene/window.yaml')
        )
        app.run()
    else:
        scene = Scene('Scene/hello.yaml')
        scene.generate_binary(target_folder='../Debug/hello.generated/', ram_base_addr = 0xF0000000)
