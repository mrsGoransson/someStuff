import abc


class IAlerter(metaclass=abc.ABCMeta):
    @abc.abstractmethod
    def __alert__(self, message):
        raise NotImplementedError
