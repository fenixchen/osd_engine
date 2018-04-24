from PIL import Image


class ImageUtil(object):
    BASE_DIR = ''

    @classmethod
    def load(cls, image_file, transparent_color):
        image_file = ImageUtil.BASE_DIR + image_file
        im = Image.open(image_file)
        width, height = im.size
        data = []
        if im.mode != 'RGBA':
            rgb_im = im.convert('RGB')
            for y in range(height):
                for x in range(width):
                    r, g, b = rgb_im.getpixel((x, y))
                    data.append(ImageUtil.make_color(r, g, b))
        else:
            pixels = list(im.getdata())
            for y in range(height):
                for x in range(width):
                    color_pixel = pixels[y * width + x]
                    data.append(ImageUtil.make_color(color_pixel[1], color_pixel[2], color_pixel[3]))

        return width, height, data

    @staticmethod
    def blend_pixel(dst, src, src_alpha):
        if src_alpha == 255:
            return src
        elif src_alpha == 0:
            return dst
        else:
            alpha = src_alpha / 255
            dst_R, dst_G, dst_B = ImageUtil.rgb(dst)
            src_R, src_G, src_B = ImageUtil.rgb(src)
            return ImageUtil.make_color(dst_R * (1 - alpha) + alpha * src_R,
                                        dst_G * (1 - alpha) + alpha * src_G,
                                        dst_B * (1 - alpha) + alpha * src_B)

    @staticmethod
    def color_clip(val):
        return min(max(0, int(val)), 255)

    @staticmethod
    def rgb(color):
        return ((int(color) >> 16) & 0xFF), ((int(color) >> 8) & 0xFF), (int(color) & 0xFF)

    @staticmethod
    def alpha(color):
        return (color >> 24) & 0xFF

    @staticmethod
    def set_alpha(color, alpha):
        a = ImageUtil.color_clip(alpha)
        return (color & 0xFFFFFF) | (a << 24)

    @staticmethod
    def make_color(r, g, b, a = 0):
        R = ImageUtil.color_clip(r)
        G = ImageUtil.color_clip(g)
        B = ImageUtil.color_clip(b)
        A = ImageUtil.color_clip(a)
        return (A << 24) | (R << 16) | (G << 8) | B

    @staticmethod
    def color_add(src, R_delta, G_delta, B_delta):
        R, G, B = ImageUtil.rgb(src)
        return ImageUtil.make_color(R + R_delta, G + G_delta, B + B_delta)


if __name__ == '__main__':
    ImageUtil.load('bird.bmp')
