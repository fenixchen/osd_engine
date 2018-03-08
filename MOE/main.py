# -*- coding:utf-8 -*-

import app
from scene import Scene

TEST_ALL = False
if __name__ == '__main__':
    if TEST_ALL:
        app = app.App(
            Scene('Scene/button.yaml'),
            Scene('Scene/form.yaml'),
            Scene('Scene/label.yaml'),
            Scene('Scene/animation.yaml'),
            Scene('scene/button.yaml'),
            Scene('scene/hello.yaml'),
            Scene('scene/line.yaml'),
            Scene('scene/menu.yaml'),
            Scene('scene/rect.yaml'),
            Scene('Scene/color.yaml'),
            Scene('scene/progressbar.yaml'),
            Scene('scene/move.yaml')
        )
        app.run()
    else:
        scene = Scene('Scene/button.yaml')
        app.App(scene).run()
        scene.generate_binary(target_folder='../Debug/hello.generated/',
                              ram_base_addr=0xF0000000)
