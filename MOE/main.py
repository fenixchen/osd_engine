# -*- coding:utf-8 -*-

import app
from scene import Scene


if __name__ == '__main__':
    if False:
        app.App().run()
    else:
        scene = Scene('Scene/move.yaml')
        scene.generate_binary(target_folder='../Debug/hello.generated/',
                              ram_base_addr=0xF0000000)
        app.App(scene).run()
    """
    if not GEN_BINARY:
        app = app.App(
            Scene('Scene/animation.yaml'),
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
        app.App(scene).run()
        scene.generate_binary(target_folder='../Debug/hello.generated/',
                              ram_base_addr=0xF0000000)
    """