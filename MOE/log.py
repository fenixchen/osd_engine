# -*- coding: UTF-8 -*-

import logging

logging.basicConfig(level=logging.DEBUG, format="[%(filename)s:%(lineno)s] %(name)s - %(levelname)s - %(message)s")


class Log(object):
    _LOG_FORMAT = "%(name)s - %(levelname)s - %(message)s"

    _logger = {
        "engine": [logging.DEBUG, None],
        "app": [logging.DEBUG, None],
    }

    @staticmethod
    def get_logger(module):
        if module in Log._logger:
            logger_tuple = Log._logger[module]
            if logger_tuple[1] is None:
                logger_tuple[1] = logging.getLogger(module)
                logger_tuple[1].setLevel(logger_tuple[0])
            return logger_tuple[1]
        else:
            raise Exception("no logger for module %s defined" % module)


if __name__ == '__main__':
    logger = Log.get_logger("MOE")
    logger.debug("hello")
    logger2 = Log.get_logger("app")
    logger2.info("world")
