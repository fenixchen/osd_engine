from PIL import Image


class ImageUtil(object):
    BASE_DIR = ''

    @classmethod
    def load(cls, image_file):
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
                    cpixel = pixels[y * width + x]
                    data.append(ImageUtil.make_color(cpixel[1], cpixel[2], cpixel[3]))

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
        return (int(color) & 0xFF), ((int(color) >> 8) & 0xFF), ((int(color) >> 16) & 0xFF)

    @staticmethod
    def make_color(r, g, b):
        R = ImageUtil.color_clip(r)
        G = ImageUtil.color_clip(g)
        B = ImageUtil.color_clip(b)
        return (B << 16) | (G << 8) | R

    @staticmethod
    def color_add(src, R_delta, G_delta, B_delta):
        R, G, B = ImageUtil.rgb(src)
        return ImageUtil.make_color(R + R_delta, G + G_delta, B + B_delta)


if __name__ == '__main__':
    ImageUtil.load('bird.bmp')
