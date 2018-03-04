from PIL import Image


class ImageUtil(object):
    BASE_DIR = ''

    @classmethod
    def load(cls, image_file):
        image_file = ImageUtil.BASE_DIR + image_file
        im = Image.open(image_file)
        width, height = im.size
        pix = im.load()
        data = []
        for y in range(height):
            for x in range(width):
                r, g, b = pix[x, y]
                data.append((r << 16) | (g << 8) | b)
        return width, height, data

    @staticmethod
    def blend_pixel(dst, src, src_alpha):
        alpha = src_alpha / 255
        if dst == 0:
            return src
        else:
            dst_R = (dst & 0xFF0000) >> 16
            dst_G = (dst & 0xFF00) >> 8
            dst_B = (dst & 0xFF)
            src_R = (src & 0xFF0000) >> 16
            src_G = (src & 0xFF00) >> 8
            src_B = (src & 0xFF)
            return (int(dst_R * (1 - alpha) + (alpha * src_R)) << 16) + (
                    int(dst_G * (1 - alpha) + (alpha * src_G)) << 8) + int(
                dst_B * (1 - alpha) + (alpha * src_B))

    @staticmethod
    def color_clip(val):
        return min(max(0, int(val)), 255)

    @staticmethod
    def rgb(color):
        return ((int(color) >> 16) & 0xFF,
                (int(color) >> 8) & 0xFF,
                int(color) & 0xFF)

    @staticmethod
    def make_color(R, G, B):
        R = ImageUtil.color_clip(R)
        G = ImageUtil.color_clip(G)
        B = ImageUtil.color_clip(B)
        return (R << 16) | (G << 8) | B

    @staticmethod
    def color_add(src, R_delta, G_delta, B_delta):
        R, G, B = ImageUtil.rgb(src)
        return ImageUtil.make_color(R + R_delta, G + G_delta, B + B_delta)


if __name__ == '__main__':
    ImageUtil.load('bird.bmp')
