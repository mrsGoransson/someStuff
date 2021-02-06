import abc


class IAlerter(metaclass=abc.ABCMeta):
    @abc.abstractmethod
    def alert(self, message):
        raise NotImplementedError
