# -*- coding:utf-8 -*-

import app
from scene import Scene

if __name__ == '__main__':
    app = app.App(
        Scene('Scene/animation.yaml'),
        Scene('scene/button.yaml'),
        Scene('scene/hello.yaml'),
        Scene('scene/line.yaml'),
        Scene('scene/menu.yaml'),
        Scene('scene/rect.yaml'),
        Scene('scene/progressbar.yaml'),
        Scene('scene/move.yaml')
    )
    app.run()
    """
    scene = Scene('Scene/hello.yaml')
    app.App(scene).run()
    scene.generate_binary(target_folder='../Debug/hello.generated/',
                          ram_base_addr=0xF0000000)
    """
